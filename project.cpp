#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <windows.h>
#include <Mmsystem.h>
#include <glut.h>

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)


using namespace std;

double L[3]={0,0,0};
double R[3]={1,0,0};
double U[3]={0,1,0};
double C[3]={0,0,0};

double thetac=0.025;
double alpha=1.0;

//double R, Rspeed, vAngle, hAngle, angleSpeed;
//for schore
int score = 0;
char scoreStr[100];
char fscore[100];

//stop and show score
bool stop = false;
bool over = false;

//for bitmap images
int num_texture = 0;
GLuint skyImage, MainShip;

//star size
float star_size = 0.04f;

//screen size
float box_ht = 1000;
float box_wid = 750;

//player position
float player_x = 0.0f;
float player_y = 1.0f;
float player_wid = 1.0f;

//screen size compare
float base_x = 0.0f;
float base_y = -6.75f;

//compare enemy size
float spi_wid = 0.6f;
float spi_ht = 0.6f;

//bullet
float bullet_x;
float bullet_y;
float bullet_speed = 0.04f;
float bullet_rad = 0.03f;
float theta = 0.0f;

//enemy
//bool stop = false;

//Create 2D Vector Structure
typedef struct Vect {
	float x;
	float y;
}Vector2D;

//Star Properties (background)
typedef struct s {
	Vector2D pos;
}Star;

Star stars[1000];
int n_stars = 100;

struct playerProperties
{
    float xPosition;
    float yPosition;
    float xSpeed;
    float ySpeed;
};
playerProperties player;

typedef struct bullet {
	Vector2D position;
	Vector2D speed;
	float angle;
	int blasted;
}BulletShell;

BulletShell bulletShell[1000000];
int n_bullets = 0;

typedef struct Enemy {
	char color;
	int state;
	Vector2D position;
	float speed;
}Enemy;

Enemy enemys[100000];
int n_enemys = 0;

void cross(double a[3], double b[3], double c[3])
{
    c[0] = a[1]*b[2] - a[2]*b[1];
    c[1] = a[2]*b[0] - a[0]*b[2];
    c[2] = a[0]*b[1] - a[1]*b[0];
}

void drawAxes()
{

        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glColor3f(1.0,0.0,0.0);
            glVertex3f( 300,0,0);
            glVertex3f(-300,0,0);

            glColor3f(0.0,1.0,0.0);
            glVertex3f(0,-300,0);
            glVertex3f(0, 300,0);

            glColor3f(0.0,0.0,1.0);
            glVertex3f(0,0, 300);
            glVertex3f(0,0,-500);
        }
        glEnd();

}

void drawBitmapText(char* string, float x, float y, float z)
{
	char* tx;
	glRasterPos3f(x,y,z);
	for (tx = string; *tx != '\0'; tx++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *tx);
	}
}

int loadBitmapImage(string ff)
{
    printf("line 1\n");
	int i, j = 0;
	FILE *l_file;
	BYTE *l_texture;

	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	RGBTRIPLE rgb;

	num_texture++;

    char filename[103];
    for(int i=0;i<ff.size();++i){
        filename[i]=ff[i];
    }
    filename[ff.size()]='\0';

	if ((l_file = fopen(filename, "rb")) == NULL)
	{
        cout<<"null\n";
		return (-1);
	}
    cout<<(int)infoheader.biWidth<<"\n";
    cout<<(int)infoheader.biHeight<<"\n";
	fread(&fileheader, sizeof(fileheader), 1, l_file);

	fseek(l_file, sizeof(fileheader), SEEK_SET);
	fread(&infoheader, sizeof(infoheader), 1, l_file);

	l_texture = (BYTE *)malloc(infoheader.biWidth * infoheader.biHeight * 4);
	memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
	for (i = 0; i < infoheader.biWidth * infoheader.biHeight; i++)
	{
		fread(&rgb, sizeof(rgb), 1, l_file);

		l_texture[j + 0] = rgb.rgbtRed;
		l_texture[j + 1] = rgb.rgbtGreen;
		l_texture[j + 2] = rgb.rgbtBlue;
		l_texture[j + 3] = 255;
		j += 4;
	}

	for(int k = 0;k<(int)sizeof(l_texture);++k){
        cout<<"l_texture["<<k<<"]: "<<(int)l_texture[k]<<"\n";
	}

	fclose(l_file);

	glBindTexture(GL_TEXTURE_2D, num_texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

	free(l_texture);

	return (num_texture);
}

void loadAllImages()
{
    printf("loading....\n");
	MainShip = loadBitmapImage("images/mainship2.bmp");
	skyImage = loadBitmapImage("images/sk2.bmp");
	cout<<"loaded....\n";
}

//Draw Star Background
void drawStar(float size) {

	glBegin(GL_TRIANGLES);
	glVertex2f(size/3, size/3);
	glVertex2f(-size/3, size/3);
	glVertex2f(0, -1.414*size/3);
	glVertex2f(size/3, -size/3);
	glVertex2f(-size/3, -size/3);
	glVertex2f(0, 1.414*size/3);
	glEnd();
}
//Create Star background
void starCreator() {


	int i;
	for (i=0; i<n_stars; i++)
	{
		Star temp;
		temp.pos.x = (-7 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(7+7))));
		temp.pos.y = (-6.9 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(6.9+6.9))));

		stars[i] = temp;
	}
}

//Draw Player
void drawPlayer(){
    double equation[4];

	equation[0] = 0;
	equation[1] = 0;
	equation[2] = 1;
	equation[3] = 0;

    glClipPlane(GL_CLIP_PLANE0, equation);
    glEnable(GL_CLIP_PLANE0);

    glEnable(GL_TEXTURE_2D);
    {
        glColor3f( 1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, MainShip);


    glPushMatrix();
    glTranslatef(player.xPosition,player.yPosition, 0.0f );
//    glBegin(GL_QUADS);
//				{
//				    glTexCoord3f(0.0, 0.0, 0.0);
//				    glVertex3f(0,0,0);
//
//				    glTexCoord3f(1.0, 0.0, 0.0);
//				    glVertex3f(10,0,0);
//
//				    glTexCoord3f(1.0, 1.0, 0.0);
//				    glVertex3f(10,10,0);
//
//				    glTexCoord3f(0.0, 1.0, 0.0);
//				    glVertex3f(0,10,0);
//				}

    glBegin( GL_TRIANGLES );

        glTexCoord3f(1.0, 1.0, 0.0);
        glVertex3f( -0.25f, -0.25f, 0.0f );
        glTexCoord3f(1.0, 1.0, 0.0);
        glVertex3f( 0.25f, -0.25f, 0.0f );
        glTexCoord3f(0.0, 1.0, 0.0);
        glVertex3f( 0.0f, 0.25f, 0.0f );


    glEnd();
    glPopMatrix();

    }
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CLIP_PLANE0);

    //glFlush();
}

//Draw Bullet
void drawBullet(float rad) {

	glBegin(GL_TRIANGLE_FAN);

	for(int i=0 ; i<360 ; i++) {
		glVertex2f(rad * cos(DEG2RAD(i)), rad * sin(DEG2RAD(i)));
	}
	glEnd();
}

void bulletCreator(float p_x, float p_y, float angle) {

	BulletShell bullet;
	bullet.position.x = p_x;
	bullet.position.y = p_y;
	bullet.speed.x = bullet_speed;
	bullet.speed.y = bullet_speed;
	bullet.angle = theta;
	bullet.blasted = 0;

	bulletShell[n_bullets++] = bullet;
}

void drawEnemy() {

	 glBegin( GL_TRIANGLES );
        glVertex3f( -0.3f, -0.3f, 0.0f );
        glVertex3f( 0.3f, -0.3f, 0.0f );
        glVertex3f( 0.0f, 0.3f, 0.0f );
     glEnd();
}
void enemyCreator(int value) {

	if (stop == false)
	{
		Enemy enem;
		enem.state = 1;
		enem.color = rand()%3;
		enem.speed = 0.004f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.004f)));
		enem.position.y = 6.9f;
		enem.position.x = (-6.85 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(6.85+6.85))));

		enemys[n_enemys++] = enem;
	}
	glutTimerFunc(2000, enemyCreator, 1);
}
void drawGrid(){
    int i;

	glColor3f(0.5, 0.5, 0.5);//gray

	glBegin(GL_LINES);
	for (i = -10; i <= 10; i++)
	{
		if (i == 0)
		{
			continue;//SKIP the MAIN axes
		}

		//lines parallel to Y-axis
		glVertex3f(i * 10, -100, 0);
		glVertex3f(i * 10, 100, 0);

		//lines parallel to X-axis
		glVertex3f(-100, i * 10, 0);
		glVertex3f(100, i * 10, 0);
	}
	glEnd();
}

void display()
{
	//clear the display
	//color black
	glClearColor(0, 0, 0, 0);

	//clear buffers to preset values
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/***************************
	/ set-up camera (view) here
	****************************/

	//load the correct matrix -- MODEL-VIEW matrix
	//specify which matrix is the current matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	//replace the current matrix with the identity matrix [Diagonals have 1, others have 0]
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//Original Look
	//gluLookAt(0,0,10,	0,0,0,	0,1,0);

	//Camera Look

	gluLookAt(C[0],C[1],C[2]+10, (C[0]+L[0]),(C[1]+L[1]),(C[2]+L[2]), U[0],U[1],U[2]);

    //Angle Change
//	{
//
//    double x = sqrt(R*R+1);
//    double y = atan(1.0/R);
//
//    double a = x*cos(vAngle+y)*cos(hAngle)-R*cos(vAngle)*cos(hAngle);
//    double b = x*cos(vAngle+y)*sin(hAngle)-R*cos(vAngle)*sin(hAngle);
//    double c = x*sin(vAngle+y)-R*sin(vAngle);
//    printf("R: %d\nRspeed: %d\nvAngle: \nhAngle: \nangleSpeed:\n",R, Rspeed, vAngle, hAngle, angleSpeed);
//
//	gluLookAt(R*cos(vAngle)*cos(hAngle),R*cos(vAngle)*sin(hAngle),R*sin(vAngle),  0,0,0,  a,b,c);
//	}

	glPushMatrix();

	if(stop == false){

    //Red enemy cross
        int n;
        for (n=0; n<n_enemys; n++)
		{
			if (enemys[n].state == 1 && enemys[n].color == 0 && (enemys[n].position.x <= player.xPosition + player_wid/2 && enemys[n].position.x >= player.xPosition - player_wid/2) && enemys[n].position.y <= player.yPosition)
			{
				stop = true;
				over = true;
			}
		}

	    //enemy out of screen destroy
        int m;
        for (m=0; m<n_enemys; m++)
        {
            if (enemys[m].state == 1 && enemys[m].position.y < base_y )
            {
                if(enemys[m].color==2){
                    score -= 2;
                }
                if(enemys[m].color==0){
                    score += 1;
                }
                enemys[m].state = 0 ;
            }
        }

        //Bullet fire and enemy destroy
        int e,f;
        for (e=0; e<n_bullets; e++)
        {
			if (bulletShell[e].blasted == 0)
			{
				for (f=0; f<n_enemys; f++)
				{
					if (enemys[f].state == 1 && enemys[f].color == 2 && enemys[f].position.y > base_y)
					{
						if (((bulletShell[e].position.x + bullet_rad >= enemys[f].position.x - spi_wid/2 && bulletShell[e].position.x + bullet_rad <= enemys[f].position.x + spi_wid/2) || (bulletShell[e].position.x - bullet_rad <= enemys[f].position.x + spi_wid/2 && bulletShell[e].position.x - bullet_rad >= enemys[f].position.x - spi_wid/2)) && (bulletShell[e].position.y + bullet_rad >=  enemys[f].position.y - spi_ht/2 && bulletShell[e].position.y + bullet_rad <= enemys[f].position.y + spi_ht/2))
						{
							enemys[f].state = 0;
							bulletShell[e].blasted = 1;
							score +=3;
							//system("laser.wav", NULL, SND_FILENAME);


						}
					}
				}
			}
        }

        //bullet destroy if out of screen
        int d;
        for (d=0; d<n_bullets; d++)
        {
			if (bulletShell[d].blasted == 0 &&(bulletShell[d].position.y + bullet_rad > box_ht/150 || bulletShell[d].position.y - bullet_rad < -box_ht/150))
			{
				bulletShell[d].blasted = 1;
			}
        }

        if (over == false)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            sprintf(scoreStr,"Score : %d",score);
            drawBitmapText(scoreStr,5, 6, 0.0f);
        }

	}





	//Draw stars
	int a;
	for (a=0; a<n_stars; a++)
	{
		glPushMatrix();
		glTranslatef(stars[a].pos.x, stars[a].pos.y, -0.1f);
		glColor3f(1.0f, 1.0f, 1.0f);
		drawStar(star_size);
		glPopMatrix();
	}

    starCreator();

	drawPlayer();

	// Draw BulletShell.

	for (a=0; a<n_bullets; a++)
	{
		if (bulletShell[a].blasted == 0)
		{
			glPushMatrix();
			glTranslatef(bulletShell[a].position.x, bulletShell[a].position.y, 0.0f);
			glColor3f(1.0f, 0.8f, 0.0f);
			drawBullet(bullet_rad);
			glPopMatrix();
		}
	}
	//moving the Bullet.
	int b;
	for (b=0; b<n_bullets; b++)
    {
        if (bulletShell[b].blasted == 0)
        {
            bulletShell[b].position.x += (bulletShell[b].speed.x*cos(DEG2RAD(PI/2 + bulletShell[b].angle)));
            bulletShell[b].position.y += (bulletShell[b].speed.y*sin(DEG2RAD(PI/2 + bulletShell[b].angle)));
        }
    }

    //Draw Enemy
    int j;
	for (j=0; j<n_enemys; j++)
	{
		if (enemys[j].state == 1)
		{
			glPushMatrix();
			glTranslatef(enemys[j].position.x, enemys[j].position.y, 0.0f);
			if (enemys[j].color == 0)
				glColor3f(1.0f, 0.0f, 0.0f);
			if (enemys[j].color == 1)
				glColor3f(0.0f, 1.0f, 0.0f);
			if (enemys[j].color == 2)
				glColor3f(1.0f, 1.0f, 1.0f);
			drawEnemy();
			glPopMatrix();
		}
	}

   // system("leaser.wav &");
   //Print Score.


	//Draw final score.
	if (over == true)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		sprintf(fscore,"Final Score:%d",score);
		drawBitmapText(fscore,0.0f, -1.0f, 0.0f);
		sprintf(scoreStr,"GameOver!");
		drawBitmapText(scoreStr,0.0f, 0.0f, 0.0f);
	}




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glPopMatrix();
	//drawAxes();
	//drawGrid();


	glutSwapBuffers();
}

void animate()
{
	//codes for any changes in Models, Camera
	int i;
    for (i=0; i<n_enemys; i++){
        enemys[i].position.y -= enemys[i].speed;
    }

	//this will call the display AGAIN
    glutPostRedisplay();
}

void keyboardListener(unsigned char key, int x,int y)
{
    player.xSpeed=0.0;
    player.ySpeed=0.0;
    if(key=='w'){
        player.ySpeed +=0.1;
        player.xPosition +=player.xSpeed;
        player.yPosition +=player.ySpeed;
    }
    if(key=='s'){
        player.ySpeed -=0.1;
        player.xPosition +=player.xSpeed;
        player.yPosition +=player.ySpeed;
    }
    if(key=='a'){
        player.xSpeed -=0.1;
        player.xPosition +=player.xSpeed;
        player.yPosition +=player.ySpeed;

    }
    if(key=='d'){
        player.xSpeed +=0.1;
        player.xPosition +=player.xSpeed;
        player.yPosition +=player.ySpeed;

    }
    if(key == 'p'){
        bulletCreator(player.xPosition, player.yPosition, 0.0);
    }


    //camera position change
    if(key=='z'){
        R[0]=cos(theta)*R[0]-sin(theta)*L[0];
        R[1]=cos(theta)*R[1]-sin(theta)*L[1];
        R[2]=cos(theta)*R[2]-sin(theta)*L[2];

        cross(U,R,L);
    }
    if(key=='x'){
        R[0]=cos(theta)*R[0]+sin(theta)*L[0];
        R[1]=cos(theta)*R[1]+sin(theta)*L[1];
        R[2]=cos(theta)*R[2]+sin(theta)*L[2];

        cross(U,R,L);
    }
    if(key=='c'){
        L[0]=cos(theta)*L[0]-sin(theta)*U[0];
        L[1]=cos(theta)*L[1]-sin(theta)*U[1];
        L[2]=cos(theta)*L[2]-sin(theta)*U[2];

        cross(R,L,U);
    }
    if(key=='v'){
        L[0]=cos(theta)*L[0]+sin(theta)*U[0];
        L[1]=cos(theta)*L[1]+sin(theta)*U[1];
        L[2]=cos(theta)*L[2]+sin(theta)*U[2];

        cross(R,L,U);
    }
}


void specialKeyListener(int key, int x, int y)
{
//Angle
//	if (key == GLUT_KEY_UP)
//	{
//		vAngle+=angleSpeed;
//	}
//	else if (key == GLUT_KEY_DOWN)
//	{
//		vAngle-=angleSpeed;
//	}
//	else if (key == GLUT_KEY_LEFT)
//	{
//		hAngle-=angleSpeed;
//	}
//	else if (key == GLUT_KEY_RIGHT)
//	{
//		hAngle+=angleSpeed;
//	}
//	else if (key == GLUT_KEY_PAGE_UP)
//	{
//	    if(R>=Rspeed){
//            R-=Rspeed;
//	    }
//	}
//	else if (key == GLUT_KEY_PAGE_DOWN)
//    {
//        R+=Rspeed;
//    }



if(key==GLUT_KEY_UP)
    {
        C[0]+=alpha*L[0];
        C[1]+=alpha*L[1];
        C[2]+=alpha*L[2];
    }
    if(key==GLUT_KEY_DOWN)
    {
        C[0]-=alpha*L[0];
        C[1]-=alpha*L[1];
        C[2]-=alpha*L[2];
    }
    if(key==GLUT_KEY_RIGHT)
    {
        C[0]+=alpha*R[0];
        C[1]+=alpha*R[1];
        C[2]+=alpha*R[2];
    }
    if(key==GLUT_KEY_LEFT)
    {
        C[0]-=alpha*R[0];
        C[1]-=alpha*R[1];
        C[2]-=alpha*R[2];
    }

}

void mouseListener(int button, int state, int x, int y)
{
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
        bulletCreator(player.xPosition, player.yPosition, 0.0);
    }
}

void init()
{
	//clear the screen
	glClearColor(0, 0, 0, 0);

	/************************
	/ set-up projection here
	************************/

	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	/*
	gluPerspective() — set up a perspective projection matrix

	fovy -         Specifies the field of view angle, in degrees, in the y direction.
	aspect ratio - Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
	zNear -        Specifies the distance from the viewer to the near clipping plane (always positive).
	zFar  -        Specifies the distance from the viewer to the far clipping plane (always positive).
	*/



	gluPerspective(70, 1, 0.1, 10000.0);

}

int main(int argc, char **argv)
{

//    R = 12.0;
//	Rspeed = 3.0;
//
//	vAngle = 90.0;
//    hAngle = 90.0;
//    angleSpeed = 0.05;

    player.xPosition = 0.0f;
    player.yPosition = -0.75f;
    player.xSpeed = 0.0;
    player.ySpeed = 0.0;

	//initialize the GLUT library
	glutInit(&argc, argv);

	glutInitWindowSize(box_ht, box_wid);
	glutInitWindowPosition(200, 1);

	/*
	glutInitDisplayMode - inits display mode
	GLUT_DOUBLE - allows for display on the double buffer window
	GLUT_RGBA - shows color (Red, green, blue) and an alpha
	GLUT_DEPTH - allows for depth buffer
	*/
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

	glutCreateWindow("Game!");

	//codes for initialization
	init();
    loadAllImages();
	//enable Depth Testing
	glEnable(GL_DEPTH_TEST);

	//display callback function

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

	//what you want to do in the idle time (when no drawing is occurring)
	glutIdleFunc(animate);

	glutTimerFunc(2000, enemyCreator, 1);

	//The main loop of OpenGL
	glutMainLoop();

	return 0;
}

