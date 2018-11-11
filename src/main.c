/*
***********************************************************
* gSTC-ISP
*
* Copyright (c) 2009-~  
*
* This source code is released for free distribution under 
* the terms of the GNU General Public License.
*
* file name: main.c
* modify date: 2010-2-9
* Author: 
*
* Program definitions:
***********************************************************
*/

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <termios.h>
#include <string.h>
#include <sys/stat.h>
#include <vte/vte.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include "main.h"
#include "uart.h"
#include "protocol.h"

#define NUMBER_OF_DEVICES 16
#define NUMBER_OF_BAUDRATES 8
#define TIMEOUT_S 5 // wait 5S



struct termios termios_save;
int ufd = -1;

int timeout = TIMEOUT_S; 
gint timeout_handl = 0;
float DOWN_BLOCK = 0;
float MAX_BLOCK = 0;
float percent = 0;
int interval = 100;

gint thandl = 0;
gint callback_handler;
gint time_handl = 0;


gchar *devices_list[NUMBER_OF_DEVICES] = {
  "/dev/ttyS0",
  "/dev/ttyS1",
  "/dev/ttyS2",
  "/dev/ttyS3",
  "/dev/tts/0",
  "/dev/tts/1",
  "/dev/tts/2",
  "/dev/tts/3",
  "/dev/ttyUSB0",
  "/dev/ttyUSB1",
  "/dev/ttyUSB2",
  "/dev/ttyUSB3",
  "/dev/usb/tts/0",
  "/dev/usb/tts/1",
  "/dev/usb/tts/2",
  "/dev/usb/tts/3"
};

gchar *baudrates_list[NUMBER_OF_BAUDRATES] = {
  "1200",
  "2400",
  "4800",
  "9600",
  "19200",
  "38400",
  "57600",
  "115200"
};

struct MainData main_data;

static int sta = 0;
static GList *pixmaps_directories = NULL;
static GtkWidget *file_choose;

void init_MainData(struct MainData *main_data);
GtkWidget* create_MainWindow(struct MainData *main_data);
gboolean on_gSTCISP_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data);
void callback_icon(GtkStatusIcon *status_icon,gpointer data);
GtkWidget*
create_pixmap (GtkWidget *widget, const gchar *filename);
GdkPixbuf* create_pixbuf (const gchar *filename);
GtkWidget* create_menu (GtkWidget *main_windown, GtkWidget *main_vbox, 
			            GtkAccelGroup *accel_group, gpointer data);
void on_quit_file_activate (GtkMenuItem	*menuitem, gpointer user_data);
void on_help_gSTCISP_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_about_gSTCISP_activate (GtkMenuItem *menuitem, gpointer user_data);

void on_button_program_clicked (GtkButton *button, gpointer user_data);
void add_pixmap_directory (const gchar *directory);
void on_button_file_clicked (GtkButton *button, gpointer user_data);
GtkWidget* create_file_choose (gpointer user_data);

void *keep_send();
int communicate(void);


void create_gSTCISP_msg(GtkWidget *mainwindow, gchar * help_msg);



int main (int argc, char *argv[])
{
	GdkPixbuf *panel_icon_pixbuf;
	GtkWidget *main_window;
	GtkStatusIcon* icon;

	gtk_set_locale ();
	gtk_init (&argc, &argv);

    add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
	add_pixmap_directory ("./pixmaps");
	add_pixmap_directory ("../pixmaps");
	
	init_MainData(&main_data);
	
	main_window = create_MainWindow(&main_data);
	
	panel_icon_pixbuf = create_pixbuf ("gSTCISP.png");
	icon = gtk_status_icon_new_from_pixbuf (panel_icon_pixbuf);
	gtk_status_icon_set_tooltip(icon,"gSTC-ISP");
	g_signal_connect((gpointer)icon, "activate", G_CALLBACK(callback_icon), main_window);
	
	gtk_widget_show (main_window);

    printf("\n****** Welcom to use gSTC-ISP *******\n");
    printf("            Good Luck\n");

	gtk_main(); 

    printf("********* gSTC-ISP exited **********\n\n");

 	return 0;
}

void init_MainData(struct MainData *maindata)
{

}

GtkWidget* create_MainWindow(struct MainData *maindata)
{
    int i = 0;
	struct stat my_stat;

    
    GtkWidget *main_window;
    GdkPixbuf *gSTCISP_icon_pixbuf;
    GtkWidget *main_vbox;
/*-------------------------main_menubar---------------------*/
	GtkWidget *main_menubar;
    GtkAccelGroup *accel_group;
    
    GtkWidget *label_setup;
    
    GtkWidget *setup_hbox;
    GtkWidget *label_port;
	GtkWidget *label_rate;
    GtkWidget *com_port;
    GtkWidget *com_rate;

    GtkWidget *label_file;
    GtkWidget *file_hbox;
    GtkWidget *label_name;
    GtkWidget *text_file;
    GtkWidget *button_file;

    GtkWidget *label_program;
    GtkWidget *program_hbox;
    GtkWidget *label_progress;
    GtkObject *adjustment_program;
    GtkWidget *bar_program;
    GtkWidget *label_percent;
    GtkWidget *button_program;


    GtkWidget *label_note;
    GtkWidget *note_vbox;
    GtkWidget *note_text;
    GtkWidget *note_alignment;
	GtkWidget *note_scrolledwindow;

	accel_group = gtk_accel_group_new ();

    main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (main_window, 400, 500);
    GTK_WIDGET_SET_FLAGS (main_window, GTK_CAN_FOCUS);
    GTK_WIDGET_SET_FLAGS (main_window, GTK_CAN_DEFAULT);
    gtk_window_set_title (GTK_WINDOW (main_window), "gSTC-ISP");
    gtk_window_set_default_size (GTK_WINDOW (main_window), 400, 500);
    gtk_window_set_destroy_with_parent (GTK_WINDOW (main_window), TRUE);
    gtk_window_set_position (GTK_WINDOW (main_window), GTK_WIN_POS_CENTER);
    gSTCISP_icon_pixbuf = create_pixbuf ("gSTCISP.png");
    if (gSTCISP_icon_pixbuf) {
        gtk_window_set_icon (GTK_WINDOW (main_window), gSTCISP_icon_pixbuf);
        gdk_pixbuf_unref (gSTCISP_icon_pixbuf);
    }
    maindata->gmain = main_window;
    

    main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (main_vbox);	
	gtk_container_add (GTK_CONTAINER (main_window), main_vbox);
	
/*------------------------main_menubar-----------------------*/
	main_menubar = create_menu(main_window, main_vbox, accel_group, &main_data);



    label_setup = gtk_label_new ("Serial Port setup :");
	gtk_widget_show (label_setup);
	gtk_box_pack_start (GTK_BOX (main_vbox), label_setup, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_setup, 100, 64);
	gtk_misc_set_alignment (GTK_MISC (label_setup), 0.03, 0.5);


    setup_hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (setup_hbox);
	gtk_box_pack_start (GTK_BOX (main_vbox), setup_hbox, FALSE, TRUE, 0);

    label_port = gtk_label_new ("Port");
	gtk_widget_show (label_port);
	gtk_box_pack_start (GTK_BOX (setup_hbox), label_port, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_port, 50, -1);
	gtk_misc_set_alignment (GTK_MISC (label_port), 0.8, 0.5);

    com_port = gtk_combo_box_entry_new_text ();
	gtk_widget_show (com_port);
	gtk_box_pack_start (GTK_BOX (setup_hbox), com_port, FALSE, TRUE, 0);	
	gtk_widget_set_size_request (com_port, 120, -1);
//    gtk_misc_set_alignment (GTK_MISC (com_port), 50, 32);

	for (i = 0; i < NUMBER_OF_DEVICES; i++) {
		if(stat(devices_list[i], &my_stat) == 0)
			//liste = g_list_append(liste, devices_list[i]);
			gtk_combo_box_append_text (GTK_COMBO_BOX (com_port),devices_list[i]);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(com_port), 0);
    maindata->com_port = com_port;


	label_rate = gtk_label_new ("Baud rate");
	gtk_widget_show (label_rate);
	gtk_box_pack_start (GTK_BOX (setup_hbox), label_rate, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_rate, 80, -1);
    gtk_misc_set_alignment (GTK_MISC (label_rate), 0.8, 0.5);


    com_rate = gtk_combo_box_entry_new_text ();
	gtk_widget_show (com_rate);
	gtk_box_pack_start (GTK_BOX (setup_hbox), com_rate, FALSE, TRUE, 0);
	gtk_widget_set_size_request (com_rate, 110, -1);
    for (i = 0; i < NUMBER_OF_BAUDRATES; i++) {
        gtk_combo_box_append_text (GTK_COMBO_BOX (com_rate),baudrates_list[i]);
	}

	gtk_combo_box_set_active(GTK_COMBO_BOX(com_rate), 3);
	gtk_entry_set_editable(GTK_ENTRY(GTK_BIN(com_rate)->child), FALSE);
    maindata->com_rate = com_rate;

    label_file = gtk_label_new ("Select File :");
	gtk_widget_show (label_file);
	gtk_box_pack_start (GTK_BOX (main_vbox), label_file, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_file, 100, 64);
	gtk_misc_set_alignment (GTK_MISC (label_file), 0.03, 0.5);

    file_hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (file_hbox);
	gtk_box_pack_start (GTK_BOX (main_vbox), file_hbox, FALSE, TRUE, 0);

    label_name = gtk_label_new ("file name");
	gtk_widget_show (label_name);
	gtk_box_pack_start (GTK_BOX (file_hbox), label_name, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_name, 80, -1);
	gtk_misc_set_alignment (GTK_MISC (label_name), 0.8, 0.5);

    text_file = gtk_entry_new ();
	gtk_widget_show (text_file);
	gtk_box_pack_start (GTK_BOX (file_hbox), text_file, FALSE, TRUE, 0);
	gtk_widget_set_size_request (text_file, 200, -1);
	gtk_entry_set_invisible_char (GTK_ENTRY (text_file), 9679);
	gtk_entry_append_text (GTK_ENTRY (text_file), "");
    maindata->text_file = text_file;

    button_file = gtk_button_new_with_mnemonic ("Select File");
	gtk_widget_show (button_file);
	gtk_box_pack_start (GTK_BOX (file_hbox), button_file, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (button_file), 0);
    gtk_widget_set_size_request (button_file, 80, 28);

    label_program = gtk_label_new ("Program :");
	gtk_widget_show (label_program);
	gtk_box_pack_start (GTK_BOX (main_vbox), label_program, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_program, 100, 64);
	gtk_misc_set_alignment (GTK_MISC (label_program), 0.03, 0.5);

    program_hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (program_hbox);
	gtk_box_pack_start (GTK_BOX (main_vbox), program_hbox, FALSE, TRUE, 0);

    label_progress = gtk_label_new ("progress");
	gtk_widget_show (label_progress);
	gtk_box_pack_start (GTK_BOX (program_hbox), label_progress, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_progress, 80, -1);
	gtk_misc_set_alignment (GTK_MISC (label_progress), 0.8, 0.5);

                                          //70.0
    adjustment_program = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 0.0, 0.0);
    bar_program = gtk_progress_bar_new_with_adjustment(GTK_ADJUSTMENT(adjustment_program));
    gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(bar_program), GTK_PROGRESS_CONTINUOUS);
    gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(bar_program), GTK_PROGRESS_LEFT_TO_RIGHT);
    gtk_widget_show (bar_program);
    gtk_box_pack_start (GTK_BOX (program_hbox), bar_program, FALSE, TRUE, 0);
	gtk_widget_set_size_request (bar_program, 165, -1);
    maindata->bar_program = bar_program;

    label_percent = gtk_label_new ("0%");
	gtk_widget_show (label_percent);
	gtk_box_pack_start (GTK_BOX (program_hbox), label_percent, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_percent, 35, -1);
	gtk_misc_set_alignment (GTK_MISC (label_percent), 0.5, 0.5);
    maindata->label_percent = label_percent; 

    button_program = gtk_button_new_with_mnemonic ("Down Load");
	gtk_widget_show (button_program);
	gtk_box_pack_start (GTK_BOX (program_hbox), button_program, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (button_program), 0);
    gtk_widget_set_size_request (button_program, 80, 28);
    maindata->button_program = button_program;


    label_note = gtk_label_new ("Note :");
	gtk_widget_show (label_note);
	gtk_box_pack_start (GTK_BOX (main_vbox), label_note, FALSE, TRUE, 0);
	gtk_widget_set_size_request (label_note, 100, 64);
	gtk_misc_set_alignment (GTK_MISC (label_note), 0.03, 0.5);

    note_vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (note_vbox);
	gtk_box_pack_start (GTK_BOX (main_vbox), note_vbox, TRUE, TRUE, 0);

    note_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (note_alignment);
    gtk_box_pack_start (GTK_BOX (note_vbox), note_alignment, TRUE, TRUE, 0);

    note_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (note_scrolledwindow);
	gtk_container_add (GTK_CONTAINER (note_alignment), note_scrolledwindow);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (note_scrolledwindow),  GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (note_scrolledwindow), GTK_SHADOW_IN);

    note_text = vte_terminal_new();
    vte_terminal_set_backspace_binding(VTE_TERMINAL(note_text),
				                       VTE_ERASE_ASCII_BACKSPACE);
	gtk_widget_show (note_text);
	gtk_container_add (GTK_CONTAINER (note_scrolledwindow), note_text);
    maindata->note_text = note_text;


    g_signal_connect ((gpointer) button_file, "clicked",
		              G_CALLBACK (on_button_file_clicked),
                      (gpointer)maindata);

    g_signal_connect ((gpointer) button_program, "clicked",
		              G_CALLBACK (on_button_program_clicked),
                      (gpointer)maindata);


    g_signal_connect ((gpointer) main_window, "delete_event",
		              G_CALLBACK (on_gSTCISP_delete_event), NULL);

    gtk_widget_grab_focus (main_window);
	gtk_widget_grab_default (main_window);

    return main_window;

}

gboolean on_gSTCISP_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{	
	if (callback_handler)
		gtk_input_remove(callback_handler);
    close_uart(ufd);
    ufd = -1;
    if (callback_handler)
        g_source_remove (callback_handler);
	gtk_main_quit();
	
	return FALSE;
}

void callback_icon(GtkStatusIcon *status_icon,gpointer data)
{
	GtkWidget *main = (GtkWidget *)data;
	if (sta == 0) {
		gtk_widget_hide(main);
		sta = 1;
	} else {
		gtk_widget_show (main);
		sta = 0;
	}
}


void add_pixmap_directory (const gchar *directory)
{
	pixmaps_directories = g_list_prepend (pixmaps_directories,
                                        g_strdup (directory));
}

static gchar* find_pixmap_file (const gchar *filename)
{
	GList *elem;

	/* We step through each of the pixmaps directory to find it. */
	elem = pixmaps_directories;
	while (elem)
	{
		gchar *pathname = g_strdup_printf ("%s%s%s", (gchar*)elem->data,
						 G_DIR_SEPARATOR_S, filename);
		if (g_file_test (pathname, G_FILE_TEST_EXISTS))
			return pathname;
		g_free (pathname);
		elem = elem->next;
	}
	return NULL;
}

GtkWidget* create_pixmap (GtkWidget *widget, const gchar *filename)
{
	gchar *pathname = NULL;
	GtkWidget *pixmap;

	if (!filename || !filename[0])
		return gtk_image_new ();

	pathname = find_pixmap_file (filename);

	if (!pathname)	{
		g_warning ("Couldn\47t find pixmap file\72 \45s", filename);
		return gtk_image_new ();
	}

	pixmap = gtk_image_new_from_file (pathname);
	g_free (pathname);
	return pixmap;
}

GdkPixbuf* create_pixbuf (const gchar *filename)
{
	gchar *pathname = NULL;
	GdkPixbuf *pixbuf;
	GError *error = NULL;

	if (!filename || !filename[0])
		return NULL;

	pathname = find_pixmap_file (filename);

	if (!pathname)	{
		g_warning ("Couldn\47t find pixmap file\72 \45s", filename);
		return NULL;
	}

	pixbuf = gdk_pixbuf_new_from_file (pathname, &error);
	if (!pixbuf) {
		fprintf (stderr, "Failed to load pixbuf file: %s: %s\n",
		pathname, error->message);
		g_error_free (error);
	}
	g_free (pathname);
	return pixbuf;
}


GtkWidget* create_menu (GtkWidget *main_windown, GtkWidget *main_vbox, 
			GtkAccelGroup *accel_group, gpointer data)
{
	GtkWidget *main_menubar;

	GtkWidget *menuitem_file;
	GtkWidget *menu_file;
	GtkWidget *quit_file;
	
	GtkWidget *menuitem_help;
	GtkWidget *menu_help;
	GtkWidget *help_gSTCISP;
	GtkWidget *about_gSTCISP;
	
	
	main_menubar = gtk_menu_bar_new ();
	gtk_widget_show (main_menubar);
	gtk_box_pack_start (GTK_BOX (main_vbox), main_menubar, FALSE, FALSE, 0);

	menuitem_file = gtk_menu_item_new_with_mnemonic ("_File");
	gtk_widget_show (menuitem_file);
	gtk_container_add (GTK_CONTAINER (main_menubar), menuitem_file);

	menu_file = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_file), menu_file);

	quit_file = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
	gtk_widget_show (quit_file);
	gtk_container_add (GTK_CONTAINER (menu_file), quit_file);


	menuitem_help = gtk_menu_item_new_with_mnemonic ("_Help");
	gtk_widget_show (menuitem_help);
	gtk_container_add (GTK_CONTAINER (main_menubar), menuitem_help);

	menu_help = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_help), menu_help);

	help_gSTCISP = gtk_menu_item_new_with_mnemonic ("_Help");
	gtk_widget_show (help_gSTCISP);
	gtk_container_add (GTK_CONTAINER (menu_help), help_gSTCISP);
	
	about_gSTCISP = gtk_menu_item_new_with_mnemonic ("_About");
	gtk_widget_show (about_gSTCISP);
	gtk_container_add (GTK_CONTAINER (menu_help), about_gSTCISP);
	
	g_signal_connect ((gpointer) quit_file, "activate",
		G_CALLBACK (on_quit_file_activate),
		NULL);
	g_signal_connect ((gpointer) help_gSTCISP, "activate",
		G_CALLBACK (on_help_gSTCISP_activate),
		(gpointer)main_windown);	
	g_signal_connect ((gpointer) about_gSTCISP, "activate",
		G_CALLBACK (on_about_gSTCISP_activate),
		(gpointer)main_windown);
	
	return main_menubar;
}


void on_quit_file_activate (GtkMenuItem	*menuitem, gpointer user_data)
{
	gtk_main_quit();
}

void on_help_gSTCISP_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	gchar help_msg[1024];
	
	snprintf(help_msg, 1024, 
		"<b>软件介绍</b>\n"
		"一个Linux下的图形化的串口ISP下载工具. \n"
        "目前仅支持对STC89C52RC(固件版本4.2C)的串口ISP编程.\n"
        "非STC官方出品,遵循GPL v2\n\n"
		"<b>软件功能限制</b>\n"
		"1.仅支持STC89C52RC(固件版本4.2C),其他芯片可能造成损坏\n"
		"2.仅支持 12/11.0592/8/6/4 MHz 外部晶振\n"
		"3.使用USB转RS232串口下载可能效果较差\n"
		"4.仅支持bin格式文件下载\n"
		"5.下载文件不能大于8192字节\n\n"
        "<b>软件操作</b>\n"
		"1.选择当前使用的串口,默认为 ttyS0\n"
		"2.选择下载波特率,默认为 9600\n"
		"3.选择下载文件\n"
		"4.关闭目标板电源\n"
		"5.按下down load键\n"
		"6.启动目标板,等待进度条达到100%%\n"
		"7.下载完成后down load键恢复为可用\n"
        "8.重复4-7步可反复下载.");

	create_gSTCISP_msg ((GtkWidget *)user_data, help_msg);

}

void on_about_gSTCISP_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *window_about;
    GdkPixbuf *about_gSTCISP_icon_pixbuf;
    
    const gchar *authors[] = {
        "walnut <walnutsoft@gmail.com>",
        NULL
    };
    const gchar *documenters[] = {
        "walnut <walnutsoft@gmail.com>",
        NULL
    };
    const gchar *artists[] = {
        "walnut <walnutsoft@gmail.com>",
        NULL
    };
    gchar translater[] = {"walnut <walnutsoft@gmail.com>"};
    gchar licence[] = {
        "gSTC-ISP 是自由软件；您可以依据自由软件基金会所发表\n"
        "的 GNU 通用公共许可证再次发布和/或修改它；\n"
        "无论您依据的是本许可证的第二版，或(您选择的)以后的任何版本。\n"
        "\n"
        "发布 gSTC-ISP 是希望它有用，但没有任何担保；\n"
        "亦无暗含的可否商业性使用或是否符合特定目的的担保。\n"
        "详情请参考 GNU 通用公共许可证。\n"
        "\n"
        "您应该在收到 gSTC-ISP 的同时收到了 GNU 通用公共许可证的副本；\n"
        "如果没有的话，请致函自由软件基金会，地址是 \n"
        "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA"
    };

    window_about = gtk_about_dialog_new();
    
    gtk_about_dialog_set_name((GtkAboutDialog *)window_about, "gSTC-ISP");
    gtk_about_dialog_set_program_name((GtkAboutDialog *)window_about, "gSTC-ISP");
    gtk_about_dialog_set_version((GtkAboutDialog *)window_about, "v0.1");
    gtk_about_dialog_set_copyright((GtkAboutDialog *)window_about, "@ 2010-2012 gSTC-ISP 作者");
    gtk_about_dialog_set_comments((GtkAboutDialog *)window_about, "图形化串口ISP工具");
    gtk_about_dialog_set_website((GtkAboutDialog *)window_about, "");
    gtk_about_dialog_set_website_label((GtkAboutDialog *)window_about, "");
    gtk_about_dialog_set_license((GtkAboutDialog *)window_about, licence);
    gtk_about_dialog_set_authors((GtkAboutDialog *)window_about,authors);
    gtk_about_dialog_set_documenters((GtkAboutDialog *)window_about, documenters);
    gtk_about_dialog_set_translator_credits((GtkAboutDialog *)window_about, translater);
    gtk_about_dialog_set_artists((GtkAboutDialog *)window_about, artists);
    
    about_gSTCISP_icon_pixbuf = create_pixbuf ("gSTCISP.png");
	if (about_gSTCISP_icon_pixbuf)
	{
		gtk_about_dialog_set_logo((GtkAboutDialog *)window_about, about_gSTCISP_icon_pixbuf);
		gdk_pixbuf_unref (about_gSTCISP_icon_pixbuf);
	}

    gtk_dialog_run(GTK_DIALOG(window_about));
    gtk_widget_destroy(window_about);

}

void on_button_file_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *window_file_choose;
	struct MainData *maindata = (struct MainData *)user_data;	
	
	maindata->choose_file = 0;
	window_file_choose = (GtkWidget *)create_file_choose(maindata);
	gtk_widget_show (window_file_choose);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
***********************************************************
* Down Load button callback
***********************************************************
*/
void on_button_program_clicked (GtkButton *button, gpointer user_data)
{
    int fd = 0;
    int nread = 0;
    int i = 0;
    gchar *port = NULL;    
    gchar *fp = NULL;
        

    struct MainData *maindata = (struct MainData *)user_data;


    fp = gtk_button_get_label(GTK_BUTTON (button));
    fp = strstr(fp, "Stop");
    if (fp != NULL) {
        printf("\nStop Button\n");
        if (time_handl) {
            gtk_timeout_remove(time_handl);
            time_handl = 0; 
        }
        gtk_progress_bar_update((GtkProgressBar *)main_data.bar_program, 0);
        gtk_button_set_label(GTK_BUTTON (button), "Down Load");
        gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
        if(callback_handler)
		    gtk_input_remove(callback_handler);
        close_uart(ufd);
        ufd = -1;
        if(callback_handler)
            g_source_remove (callback_handler);

        char buf[128];
        sprintf(buf, "Connection Canceled !\n\r");
        vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
        return;
    }


	fp = gtk_combo_box_get_active_text(GTK_COMBO_BOX(maindata->com_rate));
	down_baudrate = atoi(fp);

	port = gtk_combo_box_get_active_text(GTK_COMBO_BOX(maindata->com_port));

    fp = strstr(port, "USB");
    if (fp == NULL) {
        target_baudrate = 1200;
    } else {
        //target_baudrate = 2400;
        // for using pl2303
        target_baudrate = down_baudrate;
        if (target_baudrate > 9600) 
            target_baudrate = 9600;
    }
    
    init_msg();
    i = input_file_msg((unsigned char *)(gtk_entry_get_text(GTK_ENTRY(maindata->text_file))));

    if (i <= 0) {
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new((GtkWindow *)maindata->gmain, GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                        "File loading error\nPlease: check file to be down load");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    if (!(i % 0x80)) {
        i = i / 0x80 + 1;
    } else {
        i = i / 0x80;
    }
    MAX_BLOCK = i;
    DOWN_BLOCK = 0;
    interval = i * 0x80 * 8 * 10 / down_baudrate;
    if (interval < 50) interval = 50;

    ufd = open_uart(port, &termios_save, target_baudrate, 8, 1, 0, 0);
    if (ufd == -1) {
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new((GtkWindow *)maindata->gmain, GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                        "Open UART error\nPlease: check the UART port");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    callback_handler = gtk_input_add_full(ufd, GDK_INPUT_READ, (GdkInputFunction)communicate, NULL, NULL, NULL);
 
    //gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
    gtk_button_set_label(GTK_BUTTON (button), "Stop");
    gtk_progress_bar_update((GtkProgressBar *)maindata->bar_program, 0);

    char buf[128];
    sprintf(buf, "We are trying to connect to your MCU ...\n\r");
    vte_terminal_reset(VTE_TERMINAL(main_data.note_text), TRUE, TRUE);
    vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
   

    time_handl = gtk_timeout_add(50, (GtkFunction) keep_send, NULL); 

}

// File Choose Dialog /////////////////////////////////////

/*
***********************************************************
* The "CANCLE" callback in File Choose Dialog
***********************************************************
*/
void on_button_cancel_clicked (GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(file_choose));
}

/*
***********************************************************
* The "CANCLE" callback in File Choose Dialog
***********************************************************
*/
void on_button_ok_clicked (GtkButton *button, gpointer user_data)
{
	char *filename = NULL;
    struct stat buf;
	struct MainData *maindata = (struct MainData *)user_data;
		
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_choose));

    if (filename != NULL) {
        if (lstat(filename, &buf) < 0) { 
            return;
        }
        if (S_ISREG(buf.st_mode)) {
            gtk_entry_set_text(GTK_ENTRY(maindata->text_file), filename);
	        gtk_widget_destroy(GTK_WIDGET(file_choose)); 
        }
    }
}

/*
***********************************************************
* Choose File Dialog
***********************************************************
*/
GtkWidget* create_file_choose (gpointer user_data)
{
	GtkWidget *dialog_vbox;
	GtkWidget *dialog_action_area;
	GtkWidget *button_cancel;
	GtkWidget *button_ok;

    GtkFileFilter *file_filter_bin;
    GtkFileFilter *file_filter_all;

    struct MainData *maindata = (struct MainData *)user_data;

    file_choose = gtk_file_chooser_dialog_new ("Choose BIN file", (GtkWindow *)maindata->gmain, GTK_FILE_CHOOSER_ACTION_OPEN, NULL);

    file_filter_bin = gtk_file_filter_new();
    file_filter_all = gtk_file_filter_new();
    gtk_file_filter_set_name(file_filter_bin, "BIN files (*.bin *.BIN)");
    gtk_file_filter_add_pattern(file_filter_bin, "*.bin");
    gtk_file_filter_add_pattern(file_filter_bin, "*.BIN");
    gtk_file_filter_set_name(file_filter_all, "ALL files (*)");
    gtk_file_filter_add_pattern(file_filter_all, "*"); 

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_choose), file_filter_bin);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_choose), file_filter_all);
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(file_choose), file_filter_bin);

	dialog_vbox = GTK_DIALOG (file_choose)->vbox;
	gtk_widget_show (dialog_vbox);

	dialog_action_area = GTK_DIALOG (file_choose)->action_area;
	gtk_widget_show (dialog_action_area);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area), GTK_BUTTONBOX_END);

	button_cancel = gtk_button_new_from_stock ("gtk\55cancel");
	gtk_widget_show (button_cancel);
	gtk_dialog_add_action_widget (GTK_DIALOG (file_choose), button_cancel, GTK_RESPONSE_CANCEL);
	GTK_WIDGET_SET_FLAGS (button_cancel, GTK_CAN_DEFAULT);

	button_ok = gtk_button_new_from_stock ("gtk\55open");
	gtk_widget_show (button_ok);
	gtk_dialog_add_action_widget (GTK_DIALOG (file_choose), button_ok, GTK_RESPONSE_OK);
	GTK_WIDGET_SET_FLAGS (button_ok, GTK_CAN_DEFAULT);

	g_signal_connect ((gpointer) button_cancel, "clicked",
		    G_CALLBACK (on_button_cancel_clicked),
		    maindata);
	g_signal_connect ((gpointer) button_ok, "clicked",
		    G_CALLBACK (on_button_ok_clicked),
		    maindata);
	gtk_widget_grab_default (button_ok);

	return file_choose;
}

// End ////////////////////////////////////////////////////

/*
***********************************************************
* Error or Warring Message Dialog
***********************************************************
*/
void create_gSTCISP_msg(GtkWidget *mainwindow, gchar * help_msg)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(mainwindow),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_OTHER,
                                  GTK_BUTTONS_CLOSE,
                                  NULL);
	//gtk_widget_set_size_request (dialog, 330, -1);
	gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG(dialog), help_msg);
	gtk_widget_show(dialog);
 	g_signal_connect_swapped(dialog, "response",
                           G_CALLBACK(gtk_widget_destroy),
                           dialog);  

}

// Time Control Function //////////////////////////////////

/*
***********************************************************
* keep send 0x7F , The "START" of communicate
* note : should be in "protocol" ?
***********************************************************
*/
void *keep_send()
{	
    unsigned char buf = 0x7F;

    write_uart(ufd, &buf, 1);
//	printf("keep send\n");	
}

/*
***********************************************************
* contol the process bar
***********************************************************
*/
void *time_percent()
{
    float i = 0;
    int j = 0;
    char buf[64];
    i = DOWN_BLOCK / MAX_BLOCK;
    percent += 0.01;
    if (percent > i) percent = i;
    if (percent > 1) percent = 1;
    gtk_progress_bar_update((GtkProgressBar *)main_data.bar_program, percent);

    j = (int) (percent * 100);
    sprintf(buf, "%d%%", j);
    gtk_label_set_text(GTK_LABEL(main_data.label_percent), buf);
}

/*
***********************************************************
* do it in every second to check if waite too long
***********************************************************
*/
void *check_timeout()
{   
      
    if (timeout > 0) {
        timeout -- ;
        printf("... waite 1S\n\n", timeout);
    } else {

        printf("Communicate Error\n\n");

        if (thandl)
			gtk_timeout_remove(thandl);	
	
        //gtk_progress_bar_update((GtkProgressBar *)main_data.bar_program, 1);
        gtk_widget_set_sensitive(GTK_WIDGET(main_data.button_program), TRUE);

        if(callback_handler)
		    gtk_input_remove(callback_handler);
        close_uart(ufd);
        ufd = -1;
        if(callback_handler)
            g_source_remove (callback_handler);
        if (timeout_handl) {
            gtk_timeout_remove(timeout_handl);
            timeout_handl = 0;
        }
        char buf[128];
        sprintf(buf, "Communicate Error\n\r");
        vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));

        GtkWidget *dialog;
        dialog = gtk_message_dialog_new((GtkWindow *)main_data.gmain, GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                        "Communicat Error\nPlease: check the connection of the target system");
        gtk_window_set_title(GTK_WINDOW(dialog), "Error");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);  

     
    } 
}

// End ////////////////////////////////////////////////////

/*
***********************************************************
* between the PC and target system 
* hope this can work fine 
***********************************************************
*/
int communicate(void)
{
    unsigned char rec_buf[1024];
    int rec_len;
    unsigned char sen_buf[1024];
    int sen_len;
    unsigned char do_type = 0;
    gchar *fp = NULL;
   
    rec_len = read_uart(ufd, rec_buf, 1024);
    if (rec_len < 0) {
        printf("Read UART Error\n\n");
        return -1;
    }
    
    do_type = treat_msg(sen_buf, 1024, rec_buf, rec_len, &sen_len);
    if (do_type) {
        if (time_handl) {
            gtk_button_set_label(GTK_BUTTON (main_data.button_program), "Down Load");
            gtk_widget_set_sensitive(GTK_WIDGET(main_data.button_program), FALSE);
            gtk_timeout_remove(time_handl);
            time_handl = 0; 
        }
        if (timeout_handl == 0) {
            timeout_handl = gtk_timeout_add(1000, (GtkFunction) check_timeout, NULL);
            char buf[128];
            printf("print target information to terminal \n");
            sprintf(buf, "MCU Type is: %s\n\r", "STC89C/LE52RC");
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            sprintf(buf, "MCU Firmware Version: %s\n\r", "4.2C");
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            if (!f_double_speed) {
                sprintf(buf, "  Double speed (6 clock): %s\n\r", "No");
            } 
            else {
                sprintf(buf, "  Double speed (6 clock): %s\n\r", "Yes");
            }
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            if (!f_half_gain) {
                sprintf(buf, "  OSCDN (OSC Control): %s\n\r", "full gain");
            }
            else {
                sprintf(buf, "  OSCDN (OSC Control): %s\n\r", "1/2 gain");
            }
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            if (!f_p1_down) {
                sprintf(buf, "  P1.0, P1.1 must pull down when down load: %s\n\r", "No");
            }
            else {
                sprintf(buf, "  P1.0, P1.1 must pull down when down load: %s\n\r", "Yes");
            }
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            sprintf(buf, "  Erase data flash when erase AP: %s\n\r", "No");
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            sprintf(buf, "  Allow to access RAM(for new version, version C)\n\r");
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            sprintf(buf, "  Reset WDT if user started it(for new version, version C)\n\r");
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));

            sprintf(buf, "We are trying to run more rapid ...\n\r");
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            sprintf(buf, "Internal clock: %ld Hz .\n\r", target_hz);
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            if (!f_double_speed) {
                sprintf(buf, "External clock: %ld Hz .\n\r", target_hz);
            }
            else {
                sprintf(buf, "External clock: %ld Hz .\n\r", (target_hz >> 1));
            }
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));

        }
        timeout = TIMEOUT_S;
    }

    if (do_type == 1) { 
        printf(">>>>> Send MSG:\n");
        write_uart(ufd, sen_buf, sen_len);
    } 
    else if (do_type == 2) {
        chgb_uart(ufd, target_baudrate);

        printf(">>>>> Send MSG:\n");
    
        write_uart(ufd, sen_buf, sen_len);

        chgb_uart(ufd, down_baudrate);

    } 
    else if (do_type == 3) {
        if (DOWN_BLOCK == 0) {
            percent = 0;
            gtk_progress_bar_update((GtkProgressBar *)main_data.bar_program, 0);
            thandl = gtk_timeout_add(interval, (GtkFunction) time_percent, NULL);   

            char buf[128];
            sprintf(buf, "Now baud is: %d bps .\n\r", down_baudrate);
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
            sprintf(buf, "Programming...\n\r");
            vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
        } else {
            percent = DOWN_BLOCK / MAX_BLOCK;    
        }
        DOWN_BLOCK += 1;
        printf(">>>> Send MSG:\n");
        write_uart(ufd, sen_buf, sen_len);
   
    } 
    else if (do_type == 4) {
        if (thandl) {
			gtk_timeout_remove(thandl);	
            thandl = 0;
        }
        if (timeout_handl) {
            gtk_timeout_remove(timeout_handl);       
            timeout_handl = 0;
        } 	

        gtk_progress_bar_update((GtkProgressBar *)main_data.bar_program, 1);
        gtk_label_set_text(GTK_LABEL(main_data.label_percent), "%100");
        gtk_widget_set_sensitive(GTK_WIDGET(main_data.button_program), TRUE);
        if(callback_handler)
		    gtk_input_remove(callback_handler);
        close_uart(ufd);
        ufd = -1;
        if(callback_handler)
            g_source_remove (callback_handler);

         char buf[128];
         sprintf(buf, "Program OK\n\r");
         vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));
         sprintf(buf, "Have already encrypt.\n\r");
         vte_terminal_feed(VTE_TERMINAL(main_data.note_text), buf, strlen(buf));

    }

    return 0;
}


