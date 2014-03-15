#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <dirent.h>
#include <linux/input.h>
#include <sys/stat.h>
#include "my_fb.h"

#define	RC_0		0x00
#define	RC_1		0x01
#define	RC_2		0x02
#define	RC_3		0x03
#define	RC_4		0x04
#define	RC_5		0x05
#define	RC_6		0x06
#define	RC_7		0x07
#define	RC_8		0x08
#define	RC_9		0x09
#define	RC_RIGHT	0x0A
#define	RC_LEFT		0x0B
#define	RC_UP		0x0C
#define	RC_DOWN		0x0D
#define	RC_OK		0x0E
#define	RC_MUTE		0x0F
#define	RC_STANDBY	0x10
#define	RC_GREEN	0x11
#define	RC_YELLOW	0x12
#define	RC_RED		0x13
#define	RC_BLUE		0x14
#define	RC_PLUS		0x15
#define	RC_MINUS	0x16
#define	RC_HELP		0x17
#define	RC_DBOX		0x18
#define	RC_HOME		0x1F

#define	MAXIMAGES	9

using namespace std;
class eImage
{
public:
	std::string name;
	std::string location;
};



class meobm
{
	std::vector<eImage> imageList;
//	std::vector<eImage>::iterator it;
	static meobm *instance;
	fbClass *display;
	int rc[2];
	int selentry;
	unsigned short RCCode;
	time_t current_time;
	unsigned char *mypix;
	int mypic_x, mypic_y;

	std::string model;
	char meobootver[128];
	char kerver[128];
	char driver[128];
	char secver[128];

	void drawmenu();
	void loadImageList();
	int GetRCCode();
	void menuLoop();
	void startTimer();
	int checkTimer();
	void showpic();
	void selCurImage();
	void missionComplete();
	

 public:
	static meobm *getInstance() {return (instance) ? instance : instance = new meobm();}
	meobm();
	~meobm();
 
};

extern int fh_png_getsize(const char *, int *, int *, int, int);
extern int fh_png_load(const char *, unsigned char *, int, int);
extern int fh_png_id(const char *);

