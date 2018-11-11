


struct MainData {

    unsigned char choose_file;
	int com_stat;				//the stat of the com
	int rcv_num;				//how many byte has received
	int send_num;				//how many byte has send

	GtkWidget *gmain;
    GtkWidget *com_port;
    GtkWidget *com_rate;
    GtkWidget *text_file;
    GtkWidget *label_percent;
    GtkWidget *button_program;
    GtkWidget *bar_program;
    GtkWidget *note_text;
};

extern struct MainData main_data;
extern int communicate(void);
extern void *TEST_communicate(struct MainData *maindata);

