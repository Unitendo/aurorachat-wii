/*

                                                                                                                                                                                  
                                                                                                                                                                              
               AAA                                                                                                  WWWWWWWW                           WWWWWWWW iiii    iiii  
              A:::A                                                                                                 W::::::W                           W::::::Wi::::i  i::::i 
             A:::::A                                                                                                W::::::W                           W::::::W iiii    iiii  
            A:::::::A                                                                                               W::::::W                           W::::::W               
           A:::::::::A        uuuuuu    uuuuuu rrrrr   rrrrrrrrr      ooooooooooo   rrrrr   rrrrrrrrr   aaaaaaaaaaaaaW:::::W           WWWWW           W:::::Wiiiiiii iiiiiii 
          A:::::A:::::A       u::::u    u::::u r::::rrr:::::::::r   oo:::::::::::oo r::::rrr:::::::::r  a::::::::::::aW:::::W         W:::::W         W:::::W i:::::i i:::::i 
         A:::::A A:::::A      u::::u    u::::u r:::::::::::::::::r o:::::::::::::::or:::::::::::::::::r aaaaaaaaa:::::aW:::::W       W:::::::W       W:::::W   i::::i  i::::i 
        A:::::A   A:::::A     u::::u    u::::u rr::::::rrrrr::::::ro:::::ooooo:::::orr::::::rrrrr::::::r         a::::a W:::::W     W:::::::::W     W:::::W    i::::i  i::::i 
        A:::::A     A:::::A    u::::u    u::::u  r:::::r     r:::::ro::::o     o::::o r:::::r     r:::::r  aaaaaaa:::::a  W:::::W   W:::::W:::::W   W:::::W     i::::i  i::::i 
        A:::::AAAAAAAAA:::::A   u::::u    u::::u  r:::::r     rrrrrrro::::o     o::::o r:::::r     rrrrrrraa::::::::::::a   W:::::W W:::::W W:::::W W:::::W      i::::i  i::::i 
        A:::::::::::::::::::::A  u::::u    u::::u  r:::::r            o::::o     o::::o r:::::r           a::::aaaa::::::a    W:::::W:::::W   W:::::W:::::W       i::::i  i::::i 
        A:::::AAAAAAAAAAAAA:::::A u:::::uuuu:::::u  r:::::r            o::::o     o::::o r:::::r          a::::a    a:::::a     W:::::::::W     W:::::::::W        i::::i  i::::i 
        A:::::A             A:::::Au:::::::::::::::uur:::::r            o:::::ooooo:::::o r:::::r          a::::a    a:::::a      W:::::::W       W:::::::W        i::::::ii::::::i
            A:::::A               A:::::Au:::::::::::::::ur:::::r            o:::::::::::::::o r:::::r          a:::::aaaa::::::a       W:::::W         W:::::W         i::::::ii::::::i
            A:::::A                 A:::::Auu::::::::uu:::ur:::::r             oo:::::::::::oo  r:::::r           a::::::::::aa:::a       W:::W           W:::W          i::::::ii::::::i
            AAAAAAA                   AAAAAAA uuuuuuuu  uuuurrrrrrr               ooooooooooo    rrrrrrr            aaaaaaaaaa  aaaa        WWW             WWW           iiiiiiiiiiiiiiii
                                                                                                                                                                              
                                                                                                                                                                              
                                                                        Aurorachat rewritten for the Nintendo Wii and Gamecube                                                         
                                                                                                                                                                              
                                                                                                                                                                              
                                                                                                                                                                              
                                                                                                                                                                              



    Initial idea by: Virtualle
    Current idea by: mii-man, inspired by Virtualle
    Music by: manti-09
    Code by: Virtualle
    Comments by: Virtualle
    Backend by: Virtualle and Orstando






















*/











#include <grrlib.h>
#include <wiiuse/wpad.h>
#include <roboto_ttf.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <gccore.h>
#include <network.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <wiikeyboard/keyboard.h>

#define SERVER_IP "104.236.25.60"


/*

    http_post()
    Coded by: Virtualle
    Note: This version is very bad, and can only do HTTP, no HTTPS

*/


int http_post(const char* host, const char* path, const char* data) {
    int sock;
    struct sockaddr_in addr;
    char request[1024];
    char response[1024];

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3072);

    if (!inet_aton(host, &addr.sin_addr)) {
        return -1; 
    }

    sock = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) return -2;

    if (net_connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        net_close(sock);
        return -3;
    }

    snprintf(request, sizeof(request),
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Length: %d\r\n"
        "Content-Type: application/json\r\n"
        "Connection: close\r\n\r\n%s",
        path, host, strlen(data), data);

    if (net_send(sock, request, strlen(request), 0) < 0) {
        net_close(sock);
        return -4;
    }

    int total = 0, len;
    while ((len = net_recv(sock, response + total, sizeof(response) - total - 1, 0)) > 0) {
        total += len;
    }
    response[total] = '\0';

    if (total > 0) {
        printf("Response (%d bytes):\n%.*s\n", total, total, response);
    }

    net_close(sock);
    return 0;
}

typedef struct {
    float x, y, w, h;
    const char* label;
    char output;
} Key;

Key keys[] = {
    {  35, 100, 50, 50, "1", '1' }, {  95, 100, 50, 50, "2", '2' }, { 155, 100, 50, 50, "3", '3' },
    { 215, 100, 50, 50, "4", '4' }, { 275, 100, 50, 50, "5", '5' }, { 335, 100, 50, 50, "6", '6' },
    { 395, 100, 50, 50, "7", '7' }, { 455, 100, 50, 50, "8", '8' }, { 515, 100, 50, 50, "9", '9' }, { 575, 100, 50, 50, "0", '0' },

    {  35, 160, 50, 50, "Q", 'q' }, {  95, 160, 50, 50, "W", 'w' }, { 155, 160, 50, 50, "E", 'e' },
    { 215, 160, 50, 50, "R", 'r' }, { 275, 160, 50, 50, "T", 't' }, { 335, 160, 50, 50, "Y", 'y' },
    { 395, 160, 50, 50, "U", 'u' }, { 455, 160, 50, 50, "I", 'i' }, { 515, 160, 50, 50, "O", 'o' }, { 575, 160, 50, 50, "P", 'p' },

    {  65, 220, 50, 50, "A", 'a' }, { 125, 220, 50, 50, "S", 's' }, { 185, 220, 50, 50, "D", 'd' },
    { 245, 220, 50, 50, "F", 'f' }, { 305, 220, 50, 50, "G", 'g' }, { 365, 220, 50, 50, "H", 'h' },
    { 425, 220, 50, 50, "J", 'j' }, { 485, 220, 50, 50, "K", 'k' }, { 545, 220, 50, 50, "L", 'l' },

    {  95, 280, 50, 50, "Z", 'z' }, { 155, 280, 50, 50, "X", 'x' }, { 215, 280, 50, 50, "C", 'c' },
    { 275, 280, 50, 50, "V", 'v' }, { 335, 280, 50, 50, "B", 'b' }, { 395, 280, 50, 50, "N", 'n' },
    { 455, 280, 50, 50, "M", 'm' },

    {  35, 280, 50, 50, "Shift", '\0' },
    { 515, 280, 50, 50, ",", ',' },
    { 575, 280, 50, 50, ".", '.' },
    { 185, 340, 150, 50, "Space", ' ' },
    { 345, 340, 100, 50, "Back", '\b' },
    { 455, 340, 120, 50, "Enter", '\n' }
};

int keyCount = sizeof(keys) / sizeof(Key);

char inputText[256] = "";
int textPos = 0;




bool showkeyboard = false;

bool notjustpressed = true;

/*


    Append Chat Message
    Coded by: Virtualle
    Extra credit: Funtum
    Note: This is based off of Funtum's version, even named the same, but it is heavily different.


*/

float chatscroll = 20.0f;

char chat[3000] = "-chat-\n";


void append_message(char* to_add) {
    size_t len = strlen(chat);
    int space_left = 1000 - len - 1;

    char to_add_n[512];
    snprintf(to_add_n, sizeof(to_add_n), "%s\n", to_add);

    size_t add_len = strlen(to_add_n);

    if (add_len + 1 > space_left) {
        chat[0] = '\0';
        chatscroll = 10.0f;
    }

    int i = 0;
    while (i < add_len) {
        int chunk = (add_len - i >= 40) ? 40 : add_len - i;
        strncat(chat, to_add_n + i, chunk);
        len = strlen(chat);
        if (i + chunk < add_len && len + 2 < 1000) {
            strcat(chat, "\n");
        }
        i += chunk;
    }
    chatscroll = chatscroll - 25;
}

/*

    PrintMTTF(), prints a TTF but handles linebreaks because apparently GRRLIB doesn't do that

*/

void PrintMTTF(int x, int y, GRRLIB_ttfFont *font, const char *text, int size, u32 color, int lineSpacing) {
    char buffer[3000];
    strcpy(buffer, text);
    char *token;
    char *saveptr = NULL;
    int lineHeight = size + lineSpacing;
    int line = 0;

    token = strtok_r(buffer, "\n", &saveptr);
    while (token != NULL) {
        GRRLIB_PrintfTTF(x, y + (line * lineHeight), font, token, size, color);
        token = strtok_r(NULL, "\n", &saveptr);
        line++;
    }
}

int shiftActive = 0;

char username[21];
char password[21];

int scene = 5;

bool enteringusername = false;
bool enteringpassword = false;




int main() {
    GRRLIB_Init();
    WPAD_Init();

    // Load TTF font from the included data
    GRRLIB_ttfFont* font = GRRLIB_LoadTTF(roboto_ttf, roboto_ttf_size);

    // Enable IR and set virtual resolution
    WPAD_SetVRes(0, 640, 480);
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

    struct in_addr ip, netmask, gateway;
    int ret;

    printf("Initializing network...\n");
    ret = net_init();
    if (ret < 0) {
        printf("net_init() failed!\n");
    }

    while (if_configex(&ip, &netmask, &gateway, true, 1) < 0) {
        usleep(500000);
    }

    printf("IP: %s\n", inet_ntoa(ip));
    usleep(500000);

    int sock2;
    struct hostent *he2;
    struct sockaddr_in addr2;
    char request[1024];
    char response[1024];

    if (!inet_aton(SERVER_IP, &addr2.sin_addr)) {
        return -1;
    }

    sock2 = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock2 < 0) return -2;

    addr2.sin_family = AF_INET;
    addr2.sin_port = htons(4040);

    if (net_connect(sock2, (struct sockaddr*)&addr2, sizeof(addr2)) < 0) {
        net_close(sock2);
        return -3;
    }

    ret = http_post(SERVER_IP, "/api", "{\"cmd\":\"CONNECT\", \"version\":\"4.3\", \"platform\":\"Wii\"}");
    if (ret < 0) printf("Connect POST failed: %d\n", ret);

    char buffer[613];

    int flags = net_fcntl(sock2, F_GETFL, 0);
    net_fcntl(sock2, F_SETFL, flags | O_NONBLOCK);


    while(1) {
        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(0);
        u32 held = WPAD_ButtonsHeld(0);
        WPADData* wpaddata = WPAD_Data(0);
        

    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(sock2, &readfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000; // 10ms

    if (net_select(sock2 + 1, &readfds, NULL, NULL, &timeout) > 0) {
        ssize_t len = net_recv(sock2, buffer, sizeof(buffer)-1, 0);
        if (len > 0) {
            buffer[len] = '\0';
            append_message(buffer);
        }
    }



        if (showkeyboard) {

            if (wpaddata->ir.valid && (pressed & WPAD_BUTTON_A)) {
                float px = wpaddata->ir.x;
                float py = wpaddata->ir.y;

                for (int i = 0; i < keyCount; i++) {
                    if (px >= keys[i].x - 20 && px <= keys[i].x - 20 + keys[i].w &&
                        py >= keys[i].y - 40 && py <= keys[i].y - 40 + keys[i].h) {

                        if (keys[i].output == '\n') {
                            if (scene == 1) {
                                char message[300];
                                sprintf(message, "{\"cmd\":\"CHAT\", \"content\":\"%s\", \"username\":\"%s\", \"password\":\"%s\", \"platform\":\"Wii\"}", inputText, username, password);
                                ret = http_post(SERVER_IP, "/api", message);
                                sprintf(inputText, "");
                                textPos = 0;
                            }
                            if (scene == 2 & enteringusername || (scene == 3 & enteringusername)) {
                                sprintf(username, "%s", inputText);
                                enteringusername = false;
                                showkeyboard = false;
                                sprintf(inputText, "");
                                textPos = 0;
                            }
                            if (scene == 2 & enteringpassword || (scene == 3 & enteringpassword)) {
                                sprintf(password, "%s", inputText);
                                enteringpassword = false;
                                showkeyboard = false;
                                sprintf(inputText, "");
                                textPos = 0;
                            }
                        }
                        else if (keys[i].output == '\b') {
                            if (textPos > 0) textPos--;
                        }
                        else if (strcmp(keys[i].label, "Shift") == 0) {
                            shiftActive = !shiftActive;
                        }
                        else if (textPos < 255) {
                            char c = keys[i].output;
                            inputText[textPos++] = shiftActive ? toupper(c) : c;
                        }
                        inputText[textPos] = '\0';
                        break;
                    }
                }
            }
        }

        if (pressed & WPAD_BUTTON_HOME) {
            return 0;
            break;
        }

        GRRLIB_FillScreen(0xFFFFFFFF);

        if (scene == 5) {
            GRRLIB_Rectangle(200, 100, 250, 100, 0x888888FF, 1);
            if (wpaddata->ir.valid && (pressed & WPAD_BUTTON_A)) {
                float px = wpaddata->ir.x;
                float py = wpaddata->ir.y;
                if (!showkeyboard) {
                    if (px >= 200 - 20 && px <= 200 - 20 + 250 && py >= 100 - 40 && py <= 100 - 40 + 100) {
                        scene = 2;
                    }
                }
                if (!showkeyboard) {
                    if (px >= 200 - 20 && px <= 200 - 20 + 250 && py >= 400 - 40 && py <= 400 - 40 + 100) {
                        scene = 3;
                    }
                }
            }
            GRRLIB_Rectangle(200, 400, 250, 100, 0x888888FF, 1);
            GRRLIB_PrintfTTF(280, 130, font, "Log In", 24, 0x000000FF);
            GRRLIB_PrintfTTF(280, 450, font, "Sign Up", 24, 0x000000FF);
        }

        if (scene == 2) {
            GRRLIB_Rectangle(200, 100, 250, 100, 0x888888FF, 1);
            if (wpaddata->ir.valid && (pressed & WPAD_BUTTON_A)) {
                float px = wpaddata->ir.x;
                float py = wpaddata->ir.y;
                if (!showkeyboard) {
                    if (px >= 200 - 20 && px <= 200 - 20 + 250 && py >= 100 - 40 && py <= 100 - 40 + 100) {
                        showkeyboard = true;
                        enteringusername = true;
                    }
                }
                if (!showkeyboard) {
                    if (px >= 200 - 20 && px <= 200 - 20 + 250 && py >= 400 - 40 && py <= 400 - 40 + 100) {
                        showkeyboard = true;
                        enteringpassword = true;
                    }
                }
                if (px >= 400 - 20 && px <= 400 - 20 + 250 && py >= 400 - 40 && py <= 400 - 40 + 100) {
                    char sender[300];
                    sprintf(sender, "{\"cmd\":\"LOGINACC\", \"username\":\"%s\", \"password\":\"%s\", \"platform\":\"Wii\"}", username, password);
                    ret = http_post(SERVER_IP, "/api", sender);
                    scene = 1;
                }
            }
            GRRLIB_Rectangle(200, 400, 250, 100, 0x888888FF, 1);
            GRRLIB_PrintfTTF(280, 130, font, "Username", 24, 0x000000FF);
            GRRLIB_PrintfTTF(280, 430, font, "Password", 24, 0x000000FF);
            GRRLIB_Rectangle(400, 400, 250, 100, 0x888888FF, 1);
            GRRLIB_PrintfTTF(480, 430, font, "Log In", 24, 0x000000FF);
            GRRLIB_PrintfTTF(10, 10, font, username, 24, 0x000000FF);
            GRRLIB_PrintfTTF(10, 30, font, password, 24, 0x000000FF);
        }

        if (scene == 3) {
            GRRLIB_Rectangle(200, 100, 250, 100, 0x888888FF, 1);
            GRRLIB_PrintfTTF(280, 130, font, "Username", 24, 0x000000FF);
            if (wpaddata->ir.valid && (pressed & WPAD_BUTTON_A)) {
                float px = wpaddata->ir.x;
                float py = wpaddata->ir.y;
                if (!showkeyboard) {
                    if (px >= 200 - 20 && px <= 200 - 20 + 250 && py >= 100 - 40 && py <= 100 - 40 + 100) {
                        showkeyboard = true;
                        enteringusername = true;
                    }
                }
                if (!showkeyboard) {
                    if (px >= 200 - 20 && px <= 200 - 20 + 250 && py >= 400 - 40 && py <= 400 - 40 + 100) {
                        showkeyboard = true;
                        enteringpassword = true;
                    }
                }
                if (px >= 400 - 20 && px <= 400 - 20 + 250 && py >= 400 - 40 && py <= 400 - 40 + 100) {
                    char sender[300];
                    sprintf(sender, "{\"cmd\":\"MAKEACC\", \"username\":\"%s\", \"password\":\"%s\", \"platform\":\"Wii\"}", username, password);
                    ret = http_post(SERVER_IP, "/api", sender);
                    scene = 5;
                }
            }
            GRRLIB_Rectangle(200, 400, 250, 100, 0x888888FF, 1);
            GRRLIB_PrintfTTF(280, 430, font, "Password", 24, 0x000000FF);
            GRRLIB_Rectangle(400, 400, 250, 100, 0x888888FF, 1);
            GRRLIB_PrintfTTF(480, 430, font, "Sign Up", 24, 0x000000FF);
            GRRLIB_PrintfTTF(10, 10, font, username, 24, 0x000000FF);
            GRRLIB_PrintfTTF(10, 30, font, password, 24, 0x000000FF);
        }

        if (scene == 1) {
            PrintMTTF(20, chatscroll, font, chat, 24, 0x000000FF, 4);
        }

        if (showkeyboard) {
        
            for (int i = 0; i < keyCount; i++) {
                GRRLIB_Rectangle(keys[i].x, keys[i].y, keys[i].w, keys[i].h, 0x888888FF, 1);
                GRRLIB_Rectangle(keys[i].x, keys[i].y, keys[i].w, keys[i].h, 0x000000FF, 0);
                GRRLIB_PrintfTTF(keys[i].x + 10, keys[i].y + 15, font, keys[i].label, 24, 0xFFFFFFFF);
            }
            GRRLIB_PrintfTTF(100, 100, font, inputText, 24, 0x000000FF);
        }








        GRRLIB_PrintfTTF(500, 20, font, "AuroraWii", 24, 0x000000FF);

        if (pressed & WPAD_BUTTON_B) {
            PrintMTTF(200, 20, font, "key board am i right\n", 24, 0x000000FF, 4);
        }

        if (scene == 1) {
            if (pressed & WPAD_BUTTON_B) {
                showkeyboard = !showkeyboard;
            }

            if (held & WPAD_BUTTON_DOWN) {
                chatscroll = chatscroll - 5;
            }

            if (held & WPAD_BUTTON_UP) {
                chatscroll = chatscroll + 5;
            }
        }









        GRRLIB_Rectangle(wpaddata->ir.x, wpaddata->ir.y, 30, 60, RGBA(86, 159, 234, 255), 1);









        GRRLIB_Render();
    }

    exit:

        net_close(sock2);
        GRRLIB_FreeTTF(font);
        GRRLIB_Exit();
        return 0;
}