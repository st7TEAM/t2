#include "meobm.h"

meobm *meobm::instance;

meobm::meobm()
{
	instance = this;
	selentry = 0;
	model = "";
	char buf[256];
	std::string temps = "";
	unsigned int pos;
	FILE *f;
	display = new fbClass();
	

// Open Rc device
	int cnt=0, rc_num=0;
	rc[0]=rc[1]=-1;
	while(rc_num < 2)
	{
		struct stat s;
		char tmp[128];
		sprintf(tmp, "/dev/input/event%d", cnt);
		if (stat(tmp, &s))
			break;
// Open Remote Control
		if ((rc[rc_num]=open(tmp, O_RDONLY|O_NONBLOCK)) == -1)
		{
			printf("Meoboot <open remote control>");
			
		}
		if (ioctl(rc[rc_num], EVIOCGNAME(128), tmp) < 0)
			printf("EVIOCGNAME failed");


		if (!strstr(tmp, "remote control"))
			close(rc[rc_num]);
		else
			++rc_num;
		++cnt;
	}
	
	if (rc[0] == -1)
	{
		printf("couldnt find usable input device!!!\n");
	}


	f = fopen("/media/meoboot/MbootM/.version", "rt");
  	 if (f) {
		fgets(buf, 256, f);
		sprintf(meobootver, "--MeoBoot-- version %s <meo>", buf);
      		fclose(f);
   	}
	strcpy(driver,"[ n/a ]");
	strcpy(secver,"Enigma2");
	strcpy(kerver,"[ n/a ]");
	
	f = fopen("/var/lib/opkg/status", "rt");
   	if (f) {
		while (fgets(buf, 256, f)) {
			if (strstr(buf, "Package: vuplus-dvb-modules")) {
				fgets(buf, 256, f);
				temps = string(buf);
				pos = temps.find(' ');
				if(pos != string::npos)
					 temps = temps.substr(pos);
				pos = temps.find('\n');
				if(pos != string::npos)
					temps.replace(pos, 1, "");
				sprintf(driver, "[ %s ]", temps.c_str());
			}
				if (strstr(buf, "Package: gigablue-dvb-modules")) {
				fgets(buf, 256, f);
				temps = string(buf);
				pos = temps.find(' ');
				if(pos != string::npos)
					 temps = temps.substr(pos);
				pos = temps.find('\n');
				if(pos != string::npos)
					temps.replace(pos, 1, "");
				sprintf(driver, "[ %s ]", temps.c_str());
			}
			if (strstr(buf, "Package: dreambox-secondstage")) {
				fgets(buf, 256, f);
				temps = string(buf);
				pos = temps.find(' ');
				if(pos != string::npos)
					 temps = temps.substr(pos);
				pos = temps.find('\n');
				if(pos != string::npos)
					temps.replace(pos, 1, "");
				sprintf(secver, "[ %s ]", temps.c_str());
			}
			if (strstr(buf, "Package: kernel-image")) {
				fgets(buf, 256, f);
				temps = string(buf);
				pos = temps.find(' ');
				if(pos != string::npos)
					 temps = temps.substr(pos);
				pos = temps.find('\n');
				if(pos != string::npos)
					temps.replace(pos, 1, "");
				sprintf(kerver, "[ %s ]", temps.c_str());
			}
		}
      		fclose(f);
   	}

/*
	// Check if is 8xx and we have colors
	f = fopen("/proc/stb/info/model", "rt");
  	 if (f) {
		fgets(buf, 256, f);
		model = string(buf);
		pos = model.find('\n');
		if(pos != string::npos)
			model.replace(pos, 1, "");
      		fclose(f);
   	}
*/
// Vuduo
	model = "dm7025";	
/*
// Load background Pixmap
	std::string pic = "/usr/lib/enigma2/python/Plugins/Extensions/MeoBoot/icons/meomenu.png";
		if (fh_png_id(pic.c_str()) == 1) {
			fh_png_getsize(pic.c_str(), &mypic_x, &mypic_y, INT_MAX, INT_MAX);
			mypix = (unsigned char *)malloc(mypic_x * mypic_y * 3);
			fh_png_load(pic.c_str(), mypix, mypic_x, mypic_y);
		}

*/
// Gooooo

	loadImageList();
	selCurImage();
	showpic();
	drawmenu();
	menuLoop();

}


void meobm::menuLoop()
{

	do {
		if(checkTimer() > 10){
			GetRCCode();
			RCCode = RC_OK;
			break;
		}
		GetRCCode();
		switch (RCCode)
		{
			case RC_UP:
				--selentry;
				showpic();
				drawmenu();
				break;
			case RC_DOWN:
				++selentry;
				showpic();
				drawmenu();
				break;
		}
			
	} while (RCCode != RC_OK);
	
	missionComplete();
}


void meobm::missionComplete()
{

	display->SetMode(720, 576, 16);
	display->blit();

	/* close rc */
	GetRCCode();
	sleep(1);
	if (rc[0] != -1)
			close(rc[0]);
	if (rc[1] != -1)
			close(rc[1]);

	

	std::string curimage = "Flash";
	int lastentry = (imageList.size() -1);

	for (int i = 0; i <= lastentry; i++) {
		if (i == selentry) {
			curimage = imageList[i].name;
		}
	}
	if (FILE *f = fopen("/media/meoboot/MbootM/.meoboot", "w")){
		fprintf(f, curimage.c_str());
		fclose(f);
	}
}

void meobm::loadImageList()
{
	eImage image;
	struct stat s;

	image.name = "Flash";
	image.location  = "";
	imageList.push_back(image);


	DIR *d = opendir("/media/meoboot/MbootM/");
	if (d)
	{
		while (struct dirent *e = readdir(d))
		{
			if (strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))
			{
				std::string name = "/media/meoboot/MbootM/" + std::string(e->d_name);
				stat(name.c_str(), &s);
				if (S_ISDIR(s.st_mode))
				{
					image.name = std::string(e->d_name);
					image.location = "";
					imageList.push_back(image);
				}
			}
		}
	}
	closedir(d);

}

void meobm::selCurImage()
{

	std::string curimage = "";
	char buf[100];
	int lastentry = (imageList.size() -1);
	if (lastentry > MAXIMAGES)
		lastentry = MAXIMAGES;

	FILE *f = fopen("/media/meoboot/MbootM/.meoboot", "rt");
   	if (f) {
		fgets(buf, 100, f);
		fclose(f);
		curimage = string(buf);
		unsigned int n = curimage.find('\n');
		if(n != string::npos)
			curimage.replace(n, 1, "");
	}

	if(curimage != "") {
		for (int i = 0; i <= lastentry; i++) {
			if (imageList[i].name.c_str() == curimage)
				selentry = i;
		}

	}
}

void meobm::showpic()
{
// clear screen
	display->SetMode(720, 576, 16);
//	display->fb_display(mypix, NULL, mypic_x, mypic_y, 0, 0, 0, 0);
}

void meobm::drawmenu()
{
	startTimer();

	int firstentry = 0;
	int lastentry = (imageList.size() -1);
	
	if (lastentry > MAXIMAGES)
		lastentry = MAXIMAGES;
	
	if (selentry > lastentry) 
		selentry = 0;
	if (selentry < 0) 
		selentry = lastentry;

	int x = 120;
	int y = 100;
	

	for (int i = firstentry; i <= lastentry; i++)
	{

		if (i == selentry) {
			if(model == "dm7025")
				display->FillRect(x, (y - 20), 900, 26, 255, 0, 0);
			else
				display->FillRect(x, (y - 20), 500, 26, 0, 0, 255);
		}

		display->RenderString(imageList[i].name.c_str(), x, y, 900, fbClass::LEFT, 24, 255, 255, 255);
		y = y + 28;
	}
	

	display->RenderString(meobootver, 70, 365, 500, fbClass::LEFT, 22, 255, 255, 255);
	display->RenderString("Linux kernel version:", 70, 400, 200, fbClass::LEFT, 20, 255, 255, 255);
	if(model == "dm7025")
		display->RenderString(kerver, 400, 400, 250, fbClass::RIGHT, 20, 255, 0, 0);
	else
		display->RenderString(kerver, 400, 400, 250, fbClass::RIGHT, 20, 0, 255, 0);
	display->RenderString("Dvb drivers version:", 70, 425, 200, fbClass::LEFT, 20, 255, 255, 255);
	if(model == "dm7025")
		display->RenderString(driver, 400, 425, 300, fbClass::RIGHT, 20, 255, 0, 0);
	else	
		display->RenderString(driver, 400, 425, 250, fbClass::RIGHT, 20, 0, 255, 0);
	display->RenderString("MeoBoot is running on:", 70, 450, 300, fbClass::LEFT, 20, 255, 255, 255);
	if(model == "dm7025")
		display->RenderString(secver, 400, 450, 250, fbClass::RIGHT, 20, 255, 0, 0);
	else
		display->RenderString(secver, 400, 450, 250, fbClass::RIGHT, 20, 0, 255, 0);
	display->blit();
	

}

int meobm::GetRCCode()
{

	struct input_event ev;
	static __u16 rc_last_key = KEY_RESERVED;

	if ((read(rc[0], &ev, sizeof(ev)) == sizeof(ev)) ||
	    (rc[1] != -1 && read(rc[1], &ev, sizeof(ev)) == sizeof(ev)))
	{
		if (ev.value)
		{
			if (ev.code != rc_last_key)
			{
				rc_last_key = ev.code;

				switch (ev.code)

				{
				case KEY_UP:		RCCode = RC_UP;		break;
				case KEY_DOWN:		RCCode = RC_DOWN;	break;
				case KEY_LEFT:		RCCode = RC_LEFT;	break;
				case KEY_RIGHT:		RCCode = RC_RIGHT;	break;
				case KEY_OK:		RCCode = RC_OK;		break;
				case KEY_0:		RCCode = RC_0;		break;
				case KEY_1:		RCCode = RC_1;		break;
				case KEY_2:		RCCode = RC_2;		break;
				case KEY_3:		RCCode = RC_3;		break;
				case KEY_4:		RCCode = RC_4;		break;
				case KEY_5:		RCCode = RC_5;		break;
				case KEY_6:		RCCode = RC_6;		break;
				case KEY_7:		RCCode = RC_7;		break;
				case KEY_8:		RCCode = RC_8;		break;
				case KEY_9:		RCCode = RC_9;		break;
				case KEY_RED:		RCCode = RC_RED;	break;
				case KEY_GREEN:		RCCode = RC_GREEN;	break;
				case KEY_YELLOW:	RCCode = RC_YELLOW;	break;
				case KEY_BLUE:		RCCode = RC_BLUE;	break;
				case KEY_VOLUMEUP:	RCCode = RC_PLUS;	break;
				case KEY_VOLUMEDOWN:	RCCode = RC_MINUS;	break;
				case KEY_MUTE:		RCCode = RC_MUTE;	break;
				case KEY_HELP:		RCCode = RC_HELP;	break;
				case KEY_MENU:		RCCode = RC_DBOX;	break;
				case KEY_EXIT:		RCCode = RC_HOME;	break;
				case KEY_POWER:		RCCode = RC_STANDBY;	break;
				}
				return 1;

			}


		}

		else
		{
			RCCode = -1;
			rc_last_key = KEY_RESERVED;
		}

	}

	RCCode = -1;
	usleep(100000);

	return 0;
}

void meobm::startTimer()
{
	current_time = time(0);
}

int meobm::checkTimer()
{
	time_t new_time = time(0);
	int current = int(new_time - current_time);
	return current;
}

meobm::~meobm()
{
}

int main(int argc, char **argv)
{
	meobm::getInstance();
	return 0;
}


