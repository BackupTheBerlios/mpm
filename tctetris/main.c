/*
 * tctetris - Tiny Console Tetris
 *
 * Copyright (c) 2006 Ivo van Poorten
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

enum colors_e { black = 0, red, green, yellow, blue, magenta, cyan, white };

#define minlevel 1
#define maxlevel 100
#define minspeed 1
#define maxspeed 10
#define blocksperlevel 30
#define BORDER red
#define BG     black
#define WIDTH  10       /* multiple of 2 */
#define HEIGHT 20       /* fixed */
#define MENUX 33

typedef char shape_t[4][4];

/* #0-#2 rotate around (1,1), #3-#5 rotate once around (1,1) then copy twice
 * #6 does not rotate at all (three copies) */
static shape_t basicshapes[7] = {
    { { 1, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } },
    { { 0, 1, 1, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 0, 0 } },
    { { 0, 1, 0, 0 }, { 1, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
    { { 1, 1, 0, 0 }, { 0, 1, 1, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
    { { 0, 1, 1, 0 }, { 1, 1, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
    { { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 }, { 0, 1, 0, 0 } },
    { { 1, 1, 0, 0 }, { 1, 1, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
};
static shape_t shapes[28];

static struct termios saveti, ti;
static int gameclock = 1000 / 40; /* msec */

/* ANSI escape sequences */
static char *cl="\033[H\033[0J", *Sf="\033[3%dm",
            *Sb="\033[4%dm",     *op="\033[39;49m",
            *cm="\033[%i%d;%dH", *md="\033[1m",
            *me="\033[0m",       *mr="\033[7m";
static int li=24, co=80;
static border = BORDER, bg = BG;

/* XXX: I can put those in one array (status + (c<<1)) */
static char board[WIDTH+4+4][HEIGHT+4+4];
static char boardcolors[WIDTH+4+4][HEIGHT+4+4];

static int nexts, nextc, view = 0;
static struct {
    int s, x, y;
    int color;
} current;
static int score, speed, sspeed = 1, level;

#define setit(a,b) ( (a)==NULL ? (b) : (a) )

#define setfg(a)    printf(Sf, a)
#define setbg(a)    printf(Sb, a)
#define resetfgbg() printf(op)
#define clrscr()    printf(op); printf(cl)
#define moveto(x,y) printf(cm,0,y,x)
#define normal()    printf(me)
#define bold()      printf(md)
#define reverse()   printf(mr)
#define flush()     fflush(stdout)

#define saveterm() tcgetattr(0, &saveti)
#define restoreterm() tcsetattr(0, TCSANOW, &saveti)
#define setterm() tcgetattr(0, &ti); \
    ti.c_iflag &= ~(ICRNL | IXON); \
    ti.c_oflag &= ~(OPOST | ONLCR); \
    ti.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO | ECHONL); \
    tcsetattr(0, TCSANOW, &ti)

static void error(char *msg) {
    fprintf(stderr, "error: %s\n", msg);
    resetfgbg();
    _exit(1);
}

static int rnd(void) {
    return random()>>8;
}

static void gametick(void) {
    static int init = 0;
    static struct timeval last;
    struct timeval now;
    int prev, cur, next; /* msec */

    if (!init) {
        gettimeofday(&last, NULL);
        init = 1;
    }

    prev = last.tv_usec / 1000;
    next = prev + gameclock;
    gettimeofday(&now, NULL);
    cur = (now.tv_sec - last.tv_sec) * 1000 + now.tv_usec / 1000;
    if (next>cur) usleep( (next-cur) * 1000);
    gettimeofday(&last,NULL);
}

static void init(void) {
    char *TERM;

    srandom(time(NULL));
    if ((TERM = getenv("TERM")) == NULL) error("TERM not set");
    li = li < 0 ? 24 : li;
    co = co < 0 ? 80 : co;

    saveterm();
    setterm();
}

static void horizontal(int x1, int x2, int y) {
    int tx;
    static char l[81] = "                                                                                ";
    if (x2<x1) { tx=x2; x2=x1; x1=tx; }
    moveto(x1,y);
    l[x2-x1+1] = '\0';
    printf(l);
    l[x2-x1+1] = ' ';
}

static void vertical(int x, int y1, int y2) {
    int y, ty;
    if (y2<y1) { ty=y2; y2=y1; y1=ty; }
    for(y=y1; y<=y2; y++) {
        moveto(x,y);
        putchar(' ');
    }
}

static void rect(int x1, int y1, int x2, int y2) {
    horizontal(x1, x2, y1);
    horizontal(x1, x2, y2);
    vertical(x1, y1, y2);
    vertical(x2, y1, y2);
}

static void frect(int x1, int y1, int x2, int y2) {
    int y, ty;
    if (y2<y1) { ty=y2; y2=y1; y1=ty; }
    for (y=y1; y<=y2; y++) horizontal(x1, x2, y);
}

static void printsspeed(void) {
    normal(); setbg(red); setfg(white);
    moveto(MENUX+11, 14); printf("%02d", sspeed);
}

static int tgetchar(void) {
    char c;
    struct timeval tv;
    fd_set fds;

    tv.tv_sec = 0;
    tv.tv_usec = 1;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    if (select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) <= 0 )    return 0;
    read(STDIN_FILENO, &c, 1);
    return c;
}

static void flushstdin(void) {
    while (tgetchar() > 0) { }
}

static void stars(void) {
    int a;
    setbg(black); setfg(white);
    for (a=1; a<=50; a++) {
        moveto(rnd()%79+1,rnd()%24+1);
        printf(".");
    }
}

static void gamescreen(void) {
    clrscr();
    stars();
    setbg(blue); setfg(white);
    moveto(1,24);
    printf(" [j] left  [k] rotate  [l] right  [space] drop  [p] pause  [n] next  [q] quit ");
    setbg(border);
    frect(40-WIDTH-2,1,40+WIDTH+1,HEIGHT+1);
#define VIEWX 5
#define VIEWY 8
    setbg(yellow);
    rect(VIEWX-3,VIEWY-2,VIEWX+8,VIEWY+5);
    rect(VIEWX-2,VIEWY-2,VIEWX+7,VIEWY+5);
    setbg(bg);
    frect(VIEWX-1, VIEWY-1, VIEWX+6, VIEWY+4);
    frect(40-WIDTH,1,40+WIDTH-1,HEIGHT);
    moveto(1,24);
    flush();
}

static void clearboard(void) {
    int x, y;

    for (x=0; x<WIDTH+4+4; x++)
        for (y=0; y<HEIGHT+4+4; y++)
            board[x][y] = 0;
    for (x=0; x<4; x++)
        for (y=0; y<HEIGHT+4+4; y++)
            board[x][y] = board[WIDTH+4+x][y] = 1;
    for (x=0; x<WIDTH+4+4; x++)
        for (y=HEIGHT+4; y<HEIGHT+4+4; y++)
            board[x][y] = 1;
}

static void drawshape(int s, int color, int x, int y) {
    int a, b;

    setbg(color);
    for (a=0; a<4; a++) {
        if (y+a<=0) continue;
        for(b=0; b<4; b++) {
            if (shapes[s][a][b]) {
                moveto(x+b*2, y+a);
                printf("  ");
            }
        }
    }
}

static void printscore(void) {
    setbg(blue); setfg(white);
    moveto(1,1);
    printf(" Score: %05d ", score);
    flush();
}

static void printlevel(void) {
    setbg(blue); setfg(white);
    moveto(68,1);
    printf(" Stage: %02d ", level);
    moveto(68,3);
    printf(" Speed: %02d ", speed);
    flush();
}

static int shapefits(int s, int x, int y) {
    int a, b, c = 0;

    for (a=0; a<4; a++) {
        for (b=0; b<4; b++) {
            c += board[x+a][y+b] & shapes[s][b][a];
        }
    }
    return !(c>0);
}

static void markboard(int s, int c, int x, int y) {
    int a, b, v;

    for (a=0; a<4; a++) {
        for (b=0; b<4; b++) {
            board[x+a][y+b] |= shapes[s][b][a];
            boardcolors[x+a][y+b] = c;
        }
    }
}

static void newshape(void) {
    current.s     = nexts;
    current.x     = WIDTH / 2 + 3;
    current.y     = 1;
    current.color = nextc;
    nextc = rnd()%7 + 1;
    nexts = rnd()%28;
}

static void clearview(void) {
    normal(); setfg(black);
    frect(4,7,11,11);
}

static void drawview(int nexts, int nextc) {
    clearview();
    if (nexts == 21 || nexts == 23)
        drawshape(nexts, nextc, VIEWX-1, VIEWY);
    else if (nexts >=24)
        drawshape(nexts, nextc, VIEWX+1, VIEWY);
    else
        drawshape(nexts, nextc, VIEWX, VIEWY);
    flush();
}

static void incrscore(int i) {
    score += i;
    printscore();
}

static void incrlevel(int i) {
    level += i;
    speed += i;
    if (level > maxlevel) level = maxlevel;
    if (speed > maxspeed) speed = maxspeed;
    printlevel();
}

static void redrawboard(int y) {
    int a, b, c;

    for (b=4; b<=y; b++) {
        for (a=4; a<WIDTH+4; a++) {
            moveto(40-WIDTH-8+a*2, b-3);
            if (c = board[a][b]) setbg(boardcolors[a][b]);
            else setbg(black);
            printf("  ");
        }
    }
    flush();
}

static void removeline(int y) {
    int a, b;

    for (b=y; b>0; b--) {
        for (a=4; a<WIDTH+4; a++) {
            board[a][b] = board[a][b-1];
            boardcolors[a][b] = boardcolors[a][b-1];
        }
    }
    redrawboard(y);
}

/* only check "around" y */
static void checkboard(int y) {
    int a, b, c, r = 0, ey = y + 4;

    if (ey>=24) ey=24;

    for (b=y; b<ey; b++) {
        c = 0;
        for (a=4; a<WIDTH+4; a++) {
            if (board[a][b]) c++;
        }
        if (c == WIDTH) {
            setbg(white);
            horizontal(40-WIDTH, 40+WIDTH-1, b-3);
            moveto(1,24);
            flush();
            usleep(100000);
            setbg(black);
            horizontal(40-WIDTH, 40+WIDTH-1, b-3);
            moveto(1,24);
            flush();
            usleep(100000);
            removeline(b);
            r++;
        }
    }
    if (r==4) { /* slower if you make four rows at once! */
        speed--;
        if (speed < minspeed) speed = minspeed;
        printlevel();
    }
}

static void game(void) {
    char c;
    int counter = sspeed, rs, a, blocks = 0, paused = 0;

    gamescreen();
    clearboard();

    nexts = rnd()%28;
    nextc = rnd()%7 + 1;
    newshape();
    score = view = 0;
    if (view) drawview(nexts, nextc);
    level = 1;
    speed = sspeed;
    printscore();
    printlevel();

    for(;;) {
        if (paused) {
            gametick();
            c = tgetchar();
            switch(c) {
            case 'q':
                goto stopgame;
            case 'p':
                paused = 0;
                moveto(68,5); setbg(black);
                printf("           ");
                normal();
                flush();
            default:
                break;
            }
            continue;
        }

        if (counter <= 0) {
            counter = 11 - speed;
            if (shapefits(current.s, current.x, current.y+1)) {
                drawshape(current.s, 0, 40-WIDTH-8+current.x*2, current.y-3);
                current.y++;
                drawshape(current.s, current.color, 40-WIDTH-8+current.x*2,
                                                                current.y-3);
            } else {
                markboard(current.s, current.color, current.x, current.y);
                checkboard(current.y);
                incrscore(1);
                newshape();
                if (!shapefits(current.s, current.x, current.y)) goto stopgame;
                blocks++;
                if (blocks == blocksperlevel) {
                    blocks = 0;
                    incrlevel(1);
                }
                if (view) drawview(nexts, nextc);
            }
        }
        counter--;

        moveto(1,24);
        flush();
        gametick();
        c = tgetchar();
        switch(c) {
        case 'q':
            goto stopgame;
        case 'j':
            if (shapefits(current.s, current.x-1, current.y)) {
                drawshape(current.s, 0, 40-WIDTH-8+current.x*2, current.y-3);
                current.x--;
                drawshape(current.s, current.color, 40-WIDTH-8+current.x*2,
                                                              current.y-3);
            }
            break;
        case 'l':
            if (shapefits(current.s, current.x+1, current.y)) {
                drawshape(current.s, 0, 40-WIDTH-8+current.x*2, current.y-3);
                current.x++;
                drawshape(current.s, current.color, 40-WIDTH-8+current.x*2,
                                                              current.y-3);
            }
            break;
        case 'k':
            rs = (current.s & ~0x03) | ((current.s + 1) & 0x03);
            if (shapefits(rs, current.x, current.y)) {
                drawshape(current.s, 0, 40-WIDTH-8+current.x*2, current.y-3);
                current.s = rs;
                drawshape(current.s, current.color, 40-WIDTH-8+current.x*2,
                                                              current.y-3);
            }
            break;
        case ' ':
            for (a=1; ; a++) {
                if (!shapefits(current.s, current.x, current.y+a))
                    break;
            }
            if (shapefits(current.s, current.x, current.y+a-1)) {
                drawshape(current.s, 0, 40-WIDTH-8+current.x*2, current.y-3);
                current.y += a-1;
                drawshape(current.s, current.color, 40-WIDTH-8+current.x*2,
                                                              current.y-3);
            }
            incrscore(view ? (a-1) * 3 / 4 : a-1);
            break;
        case 'n':
            view ^= 1;
            if (view) drawview(nexts, nextc);
            else clearview();
            break;
        case 'p':
            paused = 1;
            moveto(68,5); setfg(white); setbg(blue);
            printf(" PAUSED!!! ");
            moveto(1,24);
            flush();
        default:
            break;
        }
    }

stopgame:
    return;
}

#define copyshape(a,b) memcpy(a, b, sizeof(shape_t))
#define rotateshape(a,b) { \
    a[0][0]=b[0][2]; a[0][1]=b[1][2]; a[0][2]=b[2][2]; a[0][3]=b[3][2]; \
    a[1][0]=b[0][1]; a[1][1]=b[1][1]; a[1][2]=b[2][1]; a[1][3]=b[3][1]; \
    a[2][0]=b[0][0]; a[2][1]=b[1][0]; a[2][2]=b[2][0]; a[2][3]=b[3][0]; \
}

static void genshapes(void) {
    int s, r;

    for (s=0; s<=2; s++) {
        copyshape(shapes[s*4], basicshapes[s]);
        for (r=1; r<=3; r++)
            rotateshape(shapes[s*4+r],shapes[s*4+r-1]);
    }
    for (s=3; s<=5; s++) {
        copyshape  (shapes[s*4],   basicshapes[s]);
        rotateshape(shapes[s*4+1], shapes[s*4]);
        copyshape  (shapes[s*4+2], shapes[s*4]);
        copyshape  (shapes[s*4+3], shapes[s*4+1]);
    }
    copyshape(shapes[6*4],   basicshapes[6]);
    copyshape(shapes[6*4+1], basicshapes[6]);
    copyshape(shapes[6*4+2], basicshapes[6]);
    copyshape(shapes[6*4+3], basicshapes[6]);
}

static void splash(void) {
    int r;

    clrscr();
    setbg(blue);
    horizontal(1,79,1);
    horizontal(1,79,24);
    setfg(white); setbg(white);
    rect(20,7, 60,18);
    setbg(red);
    frect(21,8, 59,17);
    bold();
    moveto(28,9);  printf("tctetris - version 0.90");
    normal(); setbg(red); setfg(white);
    moveto(23,10);  printf("Copyright (c) 2006, Ivo van Poorten");
    moveto(MENUX,12); printf("n - new game");
    moveto(MENUX,13); printf("h - high scores");
    moveto(MENUX,14); printf("s - speed [%02d]", sspeed);
    moveto(MENUX,16); printf("q - quit");
    moveto(1,23);
    flush();
}

static void starfield(void) {
    static int init = 0;
    static char sf1[22];
    int x, y;

    if (!init) {
        init ^= 1;
        for (y=0; y<22; y++) {
            sf1[y] = rnd()%79 + 1;
        }
    }

    bold(); setfg(white); setbg(black);
    for (y=2; y<=23; y++) {
        x = sf1[y-2];
        if (! (y>=7 && y<=18 && x>=20 && x<=60) ) {
            moveto(x, y);
            printf(" ");
        }
        x++;
        if (x>79) x = 1;
        if (! (y>=7 && y<=18 && x>=20 && x<=60) ) {
            moveto(x, y);
            printf(".");
        }
        sf1[y-2] = x;
    }
    normal();
    flush();
}

int main(int argc, char **argv) {
    int c;
    int x=0;

    genshapes();
    init();

    splash();

    for(;;) {
        starfield();
        moveto(1,24);
        flush();
        gametick();
        gametick();
        c = tgetchar();
        switch(c) {
        case 'n':
            game();
            splash();
            flushstdin();
            break;
        case 's':
            if (sspeed != maxspeed) sspeed++;
            printsspeed();
            break;
        case 'S':
            if (sspeed != minspeed) sspeed--;
            printsspeed();
            break;
        case 'h':
            splash();
            break;
        case 'q':
            goto out;
        default:
            break;
        }
    }

out:
    resetfgbg();
    normal();
    moveto(1,li);
    restoreterm();
    return 0;
}
