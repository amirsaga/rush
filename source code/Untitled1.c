#include<stdio.h>
#include<windows.h>
#include<time.h>
#include<SDL/SDL.h>
#include<SDL/SDL_ttf.h>
#include<SDL/SDL_mixer.h>
#define FALSE 0
#define TRUE 1
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

static int check=0;
static int gas_count=0;
static int loop_count=0;
int scores[10];
//Structure for the traffic
typedef struct TRAFF
{
        SDL_Rect TRAFFIC;
        int speed;
} TRAFF;
//<<<<<<<<<<<<<<<<<<<<<FUNCTION PROTOTYPES>>>>>>>>>>>>>>>>>>>//
void save_score(int s);
void read_scores();
void display_scores();
void sort_scores();
//function to load images
void load_files();
//function to free the surface pointers
void free();
//function to scroll the road
void scroll(SDL_Rect &, SDL_Rect &);
//function to check collisiion
int check_collision(SDL_Rect, SDL_Rect);
//functions for the positioning of the traffic
void position_traffic_up(TRAFF &, int);
void position_traffic_down(TRAFF &, int);
void reposition_traffic_up(TRAFF &, int );
void reposition_traffic_down(TRAFF &, int);
int check_position(TRAFF);
int check4cop(SDL_Rect);
//function to move the car and cop
void move_car(SDL_Rect &, int);
void move_cop(SDL_Rect &, SDL_Rect, SDL_Rect);
//////////////////////////////////////////////////////////////
//Structures for the objects
SDL_Rect BORDER;
SDL_Rect TRACK[8];
SDL_Rect TRACK1[8];
SDL_Rect CAR;
SDL_Rect COP;
SDL_Rect GAS;
SDL_Rect ROAD;
SDL_Rect SCORE;
SDL_Rect FUEL;
TRAFF T_down[4];
TRAFF T_up[4];

//Surfaces for the images
SDL_Surface* screen=NULL;
SDL_Surface* score_display=NULL;
SDL_Surface* fuel_display=NULL;
SDL_Surface* namelist=NULL;
SDL_Surface* track[8];
SDL_Surface* track1[8];
SDL_Surface* border=NULL;
SDL_Surface* startup=NULL;
SDL_Surface* car=NULL;
SDL_Surface* cop=NULL;
SDL_Surface* traffic_up[4];
SDL_Surface* traffic_down[4];
SDL_Surface* gas=NULL;
SDL_Surface* road=NULL;
SDL_Surface* car_damaged=NULL;
SDL_Surface* cop_damaged=NULL;
SDL_Surface* traffic_damaged_up[4];
SDL_Surface* traffic_damaged_down[4]; 
//pointer for the font
TTF_Font* font=NULL;
//pointer for the sound
Mix_Music* bg_music=NULL;   
Mix_Chunk* siren=NULL;
Mix_Chunk* colide=NULL;
Mix_Chunk* gas_sound=NULL;

//event structure
SDL_Event event;
SDL_Color textColor={42,240,210}; 
int main(int argc, char* args[])
{
    //initialization of SDL package
    SDL_Init(SDL_INIT_EVERYTHING);
    //initialization of font package
    TTF_Init();
    //initialization of mixer package to play sounds
    Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT,0, 4096);
    
    FILE *F;
    int quit=FALSE,collision=FALSE;
    Uint8 *keys;
    time_t sec;
    int count_siren=0;
    int count_cop=0;
    int gas_state=FALSE;
    int cop_status=FALSE;
    int pause=FALSE;
    int flag=0;
    int traff=0;
    int fuel=100;
    int score=0;
    char sc[10];        //character storage for score
    char fl[10];        //character storage for fuel percentage    
    //set screen resolution....................
    screen=SDL_SetVideoMode(510,730,32,SDL_HWSURFACE|SDL_DOUBLEBUF);//SDL_FULLSCREEN);
    //Load images................
    load_files();    
    
    read_scores();
    sort_scores();
    load_files();
    
    for(int j=0;j<8;j++)
    {
            TRACK[j].x=125;
            TRACK[j].y=90+((j-1)*90);
            TRACK1[j].x=310;
            TRACK1[j].y=90+((j-1)*90);
    }
    SCORE.x=453;
    SCORE.y=119;
    
    FUEL.x=453;
    FUEL.y=165;
    
    //attributes of the road
    ROAD.x=0;
    ROAD.y=80;
    ROAD.w=450;
    ROAD.h=650;
    //attributes of the car
    CAR.x=170;
    CAR.y=650;
    CAR.w=40;
    CAR.h=80;
    //attributes of the cop
    COP.x=170;
    COP.y=900;
    COP.w=40;
    COP.h=80;

    //attributes for the gas
    GAS.x=0;
    GAS.y=0;
    GAS.w=40;
    GAS.h=40;
    
    //color keying....................
    Uint32 colorkey;
    SDL_SetColorKey(car,SDL_SRCCOLORKEY,SDL_MapRGB(car->format,0xFF,0xFF,0xFF));
    SDL_SetColorKey(cop,SDL_SRCCOLORKEY,SDL_MapRGB(cop->format,0xFF,0xFF,0xFF));
    SDL_SetColorKey(car_damaged,SDL_SRCCOLORKEY,SDL_MapRGB(car_damaged->format,0xFF,0xFF,0xFF));
    SDL_SetColorKey(cop_damaged,SDL_SRCCOLORKEY,SDL_MapRGB(cop_damaged->format,0xFF,0xFF,0xFF));
    SDL_SetColorKey(gas,SDL_SRCCOLORKEY,SDL_MapRGB(gas->format,0xFF,0xFF,0xFF));
    SDL_SetColorKey(border,SDL_SRCCOLORKEY,SDL_MapRGB(border->format,0xFF,0xFF,0xFF));
    for(int i=0;i<4;i++)
    {
            SDL_SetColorKey(traffic_up[i],SDL_SRCCOLORKEY,SDL_MapRGB(traffic_up[i]->format,0xFF,0xFF,0xFF));
            SDL_SetColorKey(traffic_down[i],SDL_SRCCOLORKEY,SDL_MapRGB(traffic_down[i]->format,0xFF,0xFF,0xFF));
            SDL_SetColorKey(traffic_damaged_up[i],SDL_SRCCOLORKEY,SDL_MapRGB(traffic_damaged_up[i]->format,0xFF,0xFF,0xFF));
            SDL_SetColorKey(traffic_damaged_down[i],SDL_SRCCOLORKEY,SDL_MapRGB(traffic_damaged_down[i]->format,0xFF,0xFF,0xFF));
    }
    
    //loop for the front page.........
    SDL_BlitSurface(startup,NULL,screen,NULL);
    SDL_Flip(screen);
    for(int j=0;j<4;j++)
    {
            position_traffic_up(T_up[j],j);
            position_traffic_down(T_down[j],j);
    }
    Mix_PlayMusic(bg_music,-1);

    while (quit==FALSE)
    {
          while(SDL_PollEvent(&event))         //Get the event as the condition for the loop    
          {
                 if (event.type==SDL_QUIT || event.key.keysym.sym==SDLK_e)
                 {
                    quit=TRUE;
                 }
          }
          if(event.key.keysym.sym==SDLK_s)
          {
                break;
          }
          if(event.key.keysym.sym==SDLK_h)
          {
                display_scores();
                SDL_BlitSurface(startup,NULL,screen,NULL);
                SDL_Flip(screen);
          }
    }
    
    //main loop to control the program......................  
    
    while (quit==FALSE)
    {
          while(SDL_PollEvent(&event))         //Get the event as the condition for the loop    
          {
                 if (event.type==SDL_QUIT||keys[SDLK_ESCAPE])
                 {
                    quit=TRUE;
                 }
          }
          
          collision=check_collision(COP,CAR);                //CHECK COLLISION BETWEEN COP AND CAR
          if (collision==TRUE)
          {
           Mix_PlayChannel(-1,colide,0);
           flag=1;
           quit=TRUE;
          }
          
          collision=check_collision(GAS,CAR);                //CHECK COLLISION BETWEEN GAS AND CAR
          if (collision==TRUE)
          {
           Mix_PlayChannel(-1,gas_sound,0);
           score+=10;
           GAS.y=0;
           fuel+=16;
           if(fuel>100)
           {
            fuel=100;
           }
           gas_state=FALSE;
          }

           for(int j=0;j<4;j++)
           {
            collision=check_collision(T_up[j].TRAFFIC,CAR);     //CHECK COLLISION BETWEEN CAR AND TRAFFIC
            if(collision==TRUE)
            {
             Mix_PlayChannel(-1,colide,0);
             check=j;
             flag=2;
             save_score(score);
             quit=TRUE;
            }
            collision=check_collision(T_down[j].TRAFFIC,CAR);              
            if(collision==TRUE)
            {
             Mix_PlayChannel(-1,colide,0);
             check=j;
             flag=3;
             save_score(score);
             quit=TRUE;
            }
           }
          
          if(COP.y<730)
          {
           for(int j=0;j<4;j++)
           {
            collision=check_collision(T_up[j].TRAFFIC,COP);     //CHECK COLLISION BETWEEN COP AND TRAFFIC
            if(collision==TRUE)
            { 
              Mix_PlayChannel(-1,colide,0);
              flag=4;
              check=j;
              T_up[j].speed=3;
            }
            collision=check_collision(T_down[j].TRAFFIC,COP);     //CHECK COLLISION BETWEEN COP AND TRAFFIC
            if(collision==TRUE)
            { 
              Mix_PlayChannel(-1,colide,0);
              flag=5;
              check=j;
            }
           }
          }
          
//<<<<<<<<<<<<<<<<<<<<<<<<  KEY EVENTS  >>>>>>>>>>>>>>>>>>>>>>>>>>//
          keys=SDL_GetKeyState(NULL);
          
          //loop to pause the game
          if(keys[SDLK_p])
          {
           pause=TRUE;
           Mix_PauseMusic();
           while(pause==TRUE)
           {
            while(SDL_PollEvent(&event))         //Get the event as the condition for the loop    
            {
             if(event.key.keysym.sym==SDLK_r)
             {
              Mix_ResumeMusic();
              pause=FALSE;                
             }
            }
           }
          }
          if(keys[SDLK_1])
          {
           Mix_PauseMusic();
          }
          if(event.key.keysym.sym==SDLK_2)
          {
              Mix_ResumeMusic();
          }
          
          if(fuel!=0)
          {
           if(keys[SDLK_LEFT])
           {
                if(CAR.x>30)
                {
                 SDL_BlitSurface(road,&CAR,screen,&CAR);       //replace the old position of car by the bg
                 move_car(CAR,LEFT);
                }
           }
           if(keys[SDLK_RIGHT])
           {
                if(CAR.x<=375)
                {
                 SDL_BlitSurface(road,&CAR,screen,&CAR);
                 move_car(CAR,RIGHT);
                }                                    
           }
           if(keys[SDLK_UP])
           {
                if(CAR.y>=80)
                {
                SDL_BlitSurface(road,&CAR,screen,&CAR);
                move_car(CAR,UP);
                }
           }
           if(keys[SDLK_DOWN])
           {
                 SDL_BlitSurface(road,&CAR,screen,&CAR);
                 move_car(CAR,DOWN);
                 if(CAR.y>=720)
                 {
                  quit=TRUE;
                 }                               
           }
          }
          else
          {
              CAR.y+=3;
              if(CAR.y>=720)
              {
               quit=TRUE;
              }
          }          
//<<<<<<<<<<<<<<<<<<<<<<  SCORE PRINTING  >>>>>>>>>>>>>>>>>>>>>>>>>>//
loop_count+=1;
if(loop_count==50)
{
 score+=5;
 itoa(score,sc,10);
 loop_count=0;
}
score_display=TTF_RenderText_Solid( font, sc, textColor );

//<<<<<<<<<<<<<<<<<<<<<<<<  COP MOVEMENT  >>>>>>>>>>>>>>>>>>>>>>>>//
    count_cop+=1;
    if(cop_status==TRUE)
    {
     if(COP.y==0 || COP.y==950)
     {
                cop_status=FALSE;
                COP.y=900;
                flag=0;
     }
     if(flag!=4 && flag!=5)
     {
                int y=check4cop(COP);
                move_cop(COP,T_up[y].TRAFFIC,CAR);
                COP.y-=1;
     }
     else
     {
         COP.y+=3;
     }
     if(COP.y<=780 && (flag!=4 && flag!=5))
     {
                  count_siren+=1;
                  if(count_siren==30)
                  {
                   Mix_PlayChannel(-1,siren,0);
                   count_siren=0;
                  }
     }
    }
    else if(score>=100 && count_cop%10==0)
    {
         cop_status=TRUE;
    }
//<<<<<<<<<<<<<<<<<<<<<<<<  TRAFFIC MOVEMENT  >>>>>>>>>>>>>>>>>>>>>>//
for(int j=0;j<4;j++)
{
        int y=0;
        T_up[j].TRAFFIC.y+=T_up[j].speed;
        for(int i=0;i<4;i++)
        {
        y=(check_position(T_up[i]));
        if(y==0)
        {
        break;
        }
        }            
        if(y==1)
        {
                reposition_traffic_up(T_up[j],j);
        }
        T_down[j].TRAFFIC.y+=T_down[j].speed;
        for(int i=0;i<4;i++)
        {
        y=(check_position(T_down[i]));
        if(y==0)
        {
        break;
        }
        }            
        if(y==1)
        {
                reposition_traffic_down(T_down[j],j);
        }
}


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  GAS  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
    gas_count+=1;
    if(gas_count%30==0)
    {
     fuel-=1;
     if(fuel<=0)
     {
       fuel=0;
     }
    }
    if(gas_state==TRUE)
    {
                    GAS.x=(sec%10+2)*34;
                    GAS.y+=3;
                    SDL_BlitSurface(road,&GAS,screen,&GAS);
    }
    if(gas_state==FALSE)
    { 
     if(gas_count%450==0)
     {
      gas_state=TRUE;
      time(&sec);
     }
    }
    if(GAS.y>=730)
    {
     GAS.y=0;
     gas_state=FALSE;
    }
    itoa(fuel,fl,10);
    fuel_display=TTF_RenderText_Solid( font, fl, textColor );

//<<<<<<<<<<<<<<<<<<<<<<<<  SCROLLING OF ROAD  >>>>>>>>>>>>>>>>>>>>>>//
    for(int j=0;j<8;j++)
    {
            SDL_BlitSurface(road,&TRACK[j],screen,&TRACK[j]);
            SDL_BlitSurface(road,&TRACK1[j],screen,&TRACK1[j]);
            scroll(TRACK[j], TRACK1[j]);
    }
    
//////////////////////////////////////////////////////////////////////
          
          SDL_BlitSurface(road,NULL,screen,&ROAD);
          for(int j=0;j<8;j++)
          {
                SDL_BlitSurface(track[j],NULL,screen,&TRACK[j]);
                SDL_BlitSurface(track1[j],NULL,screen,&TRACK1[j]);
          }       
          SDL_BlitSurface(gas,NULL,screen,&GAS);   
          SDL_BlitSurface(car,NULL,screen,&CAR);
          if(flag==4 || flag==5)
          {
                    SDL_BlitSurface(cop_damaged,NULL,screen,&COP);
          }
          else
          {
                    SDL_BlitSurface(cop,NULL,screen,&COP);
          }
          for(int j=0;j<4;j++)
          {
           SDL_BlitSurface(traffic_up[j],NULL,screen,&T_up[j].TRAFFIC);
           SDL_BlitSurface(traffic_down[j],NULL,screen,&T_down[j].TRAFFIC);
          }
          SDL_BlitSurface(border,NULL,screen,&BORDER);
          SDL_BlitSurface(score_display,NULL,screen,&SCORE);
          SDL_BlitSurface(fuel_display,NULL,screen,&FUEL);
          SDL_Flip(screen);
          SDL_Delay(12);
    }
    //out of main loop   
    if(flag==1)
    {
       SDL_BlitSurface(car_damaged,NULL,screen,&CAR);
       SDL_BlitSurface(cop_damaged,NULL,screen,&COP);
       quit=FALSE;
    }
    else if(flag==2)
    {
       SDL_BlitSurface(car_damaged,NULL,screen,&CAR);
       SDL_BlitSurface(traffic_damaged_up[check],NULL,screen,&T_up[check].TRAFFIC);
       quit=FALSE;
    }
    else if(flag==3)
    {
       SDL_BlitSurface(car_damaged,NULL,screen,&CAR);
       SDL_BlitSurface(traffic_damaged_down[check],NULL,screen,&T_down[check].TRAFFIC);
       quit=FALSE;
    }    
    SDL_Flip(screen);
    MessageBox(NULL,"YOU LOOSE!!!!!!!!!!","GAME OVER...",MB_OK); 
    display_scores();
    SDL_Delay(500);
    free();
    TTF_Quit();
    SDL_Quit();         //quit SDL
    return 0;
}

//<<<<<<<<<<<<<<<<<<<<<<<<<<FUNCTION DEFINITION>>>>>>>>>>>>>>>>>>>>//
//load images
void load_files()
{
     bg_music=Mix_LoadMUS("sounds/title.wav");
     siren=Mix_LoadWAV("sounds/siren.wav");
     colide=Mix_LoadWAV("sounds/collide.wav");
     gas_sound=Mix_LoadWAV("sounds/gas_sound.wav");
     font=TTF_OpenFont("font/digifaw.ttf",13);
     for(int x=0;x<8;x++)
     {
             track[x]=SDL_DisplayFormat(SDL_LoadBMP("images/track.bmp"));
             track1[x]=SDL_DisplayFormat(SDL_LoadBMP("images/track.bmp"));
     }
     border=SDL_DisplayFormat(SDL_LoadBMP("images/border.bmp"));
     startup=SDL_DisplayFormat(SDL_LoadBMP("images/startup.bmp"));
     car=SDL_DisplayFormat(SDL_LoadBMP("images/car.bmp")); //load the car
     cop=SDL_DisplayFormat(SDL_LoadBMP("images/cop.bmp")); //load the cop
     road=SDL_DisplayFormat(SDL_LoadBMP("images/road.bmp")); //load the road
     gas=SDL_DisplayFormat(SDL_LoadBMP("images/gas.bmp")); //load the gas
     traffic_up[0]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic0.bmp")); //load the traffic
     traffic_up[1]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic1.bmp"));
     traffic_up[2]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic2.bmp"));
     traffic_up[3]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic1.bmp"));
     /*traffic_up[4]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic0.bmp"));
     traffic_up[5]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic2.bmp"));*/
     traffic_down[0]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic0_down.bmp")); 
     traffic_down[1]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic1_down.bmp"));
     traffic_down[2]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic2_down.bmp"));
     traffic_down[3]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic2_down.bmp"));
     /*traffic_down[4]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic1_down.bmp"));
     traffic_down[5]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic0_down.bmp"));*/
     car_damaged=SDL_DisplayFormat(SDL_LoadBMP("images/car_damaged.bmp")); //load the damaged car
     cop_damaged=SDL_DisplayFormat(SDL_LoadBMP("images/cop_damaged.bmp")); //load the damaged cop
     traffic_damaged_up[0]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic0_damaged.bmp")); //load the damaged traffic
     traffic_damaged_up[1]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic1_damaged.bmp"));
     traffic_damaged_up[2]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic2_damaged.bmp"));
     traffic_damaged_up[3]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic1_damaged.bmp"));
     /*traffic_damaged_up[4]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic0_damaged.bmp"));
     traffic_damaged_up[5]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic2_damaged.bmp"));*/
     traffic_damaged_down[0]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic0_down_damaged.bmp")); //load the damaged traffic
     traffic_damaged_down[1]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic1_down_damaged.bmp"));
     traffic_damaged_down[2]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic2_down_damaged.bmp"));
     traffic_damaged_down[3]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic2_down_damaged.bmp"));
     /*traffic_damaged_down[4]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic1_down_damaged.bmp"));
     traffic_damaged_down[5]=SDL_DisplayFormat(SDL_LoadBMP("images/traffic0_down_damaged.bmp"));*/
}   
void free()
{
     SDL_FreeSurface(screen);
     SDL_FreeSurface(score_display);
     SDL_FreeSurface(border);
     SDL_FreeSurface(startup);
     SDL_FreeSurface(car);
     SDL_FreeSurface(cop);
     SDL_FreeSurface(road);
     SDL_FreeSurface(gas);
     SDL_FreeSurface(car_damaged);
     SDL_FreeSurface(cop_damaged);
     SDL_FreeSurface(screen);
     SDL_FreeSurface(namelist);
     for(int i=0;i<6;i++)
     {
             SDL_FreeSurface(traffic_up[i]);
             SDL_FreeSurface(traffic_down[i]);
             SDL_FreeSurface(traffic_damaged_up[i]);
             SDL_FreeSurface(traffic_damaged_down[i]);
     }
     for(int i=0;i<8;i++)
     {
             SDL_FreeSurface(track[i]);
             SDL_FreeSurface(track1[i]);
     }
     TTF_CloseFont(font);
     Mix_FreeMusic(bg_music);
     Mix_FreeChunk(colide);
     Mix_FreeChunk(siren);
     Mix_FreeChunk(gas_sound);
}
//Function definition for scrolling
void scroll(SDL_Rect &track_a, SDL_Rect &track_b)
{
     track_a.y+=3;
     track_b.y+=3;
     if(track_a.y>=730)
     {
      track_a.y=0;
      track_b.y=0;
     }
}
     
//Function definition to check for collision
int check_collision(SDL_Rect a, SDL_Rect b)
{
         //The attributes of first object
         int left_a,right_a;
         int top_a,bottom_a;
         //The attributes of first object
         int top_b,bottom_b;
         int left_b,right_b;
         //Calculate the sides
         left_a=a.x;
         right_a=a.x+a.w;
         top_a=a.y;
         bottom_a=a.y+a.h;
         //Calculate the sides
         left_b=b.x;
         right_b=b.x+b.w;
         top_b=b.y;
         bottom_b=b.y+b.h;
         //test for conditions for the collision
         if(bottom_a>=top_b)
         {
          if(right_b<(right_a+b.w) && right_b>(right_a-a.w))
          {
           if(bottom_b>=top_a)
           {
            return TRUE;
           }
          }
         }
    //If test fails
    return FALSE;
}
/*The function checks whether the collision has ocured or not 
  by detecting if the rectangle of the two objects overlaps with
  each other or not*/

void move_car(SDL_Rect &a,int direction)
{
     if(direction==UP)
     {
      a.y-=3;
     }
     else if(direction==DOWN)
     {
      a.y+=3;
     }
     else if(direction==LEFT)
     {
      a.x-=2;
     }
     else if(direction==RIGHT)
     {
      a.x+=2;
     }
}

void move_cop(SDL_Rect &a,SDL_Rect b,SDL_Rect c)
{
      if((a.x==(b.x+41)) && (a.y>=(b.y-82) && a.y<=(b.y+82)))
      {
       if(a.x<c.x)
       {
        a.x+=1;
       }
      }
      else if((a.x==(b.x-41)) && (a.y<(b.y+82) && a.y>(b.y-82)))
      {
       if(a.x>c.x)
       {
        a.x-=1;
       }
      }
      else
      { 
       if(a.x<c.x)
       {
        a.x+=1;
       }
       else if(a.x>c.x)
       {
        a.x-=1;
       }
       else if(a.x==c.x)
       {
        a.x=c.x;
       }
    }
}

void position_traffic_up(TRAFF &T, int j)
{
           switch (j)
           {
                  case 0:
                       {
                       T.speed=3;
                       T.TRAFFIC.y=0;
                       T.TRAFFIC.x=40;
                       break;
                       }
                  case 1:
                       {
                       T.speed=2;
                       T.TRAFFIC.y=139;
                       T.TRAFFIC.x=105;
                       break;
                       }
                  case 2:
                       {
                       T.speed=1;
                       T.TRAFFIC.y=735;
                       T.TRAFFIC.x=170;
                       break;
                       }
                  case 3:
                       {
                       T.speed=3;
                       T.TRAFFIC.y=200;
                       T.TRAFFIC.x=40;
                       break;
                       }
           }
}          

void position_traffic_down(TRAFF &T, int j)
{
           switch (j)
           {
                  case 0:
                       {
                       T.speed=5;
                       T.TRAFFIC.y=50;
                       T.TRAFFIC.x=225;
                       break;
                       }
                  case 1:
                       {
                       T.speed=4;
                       T.TRAFFIC.y=391;
                       T.TRAFFIC.x=290;
                       break;
                       }
                  case 2:
                       {
                       T.speed=3;
                       T.TRAFFIC.y=300;
                       T.TRAFFIC.x=355;
                       break;
                       }
                  case 3:
                       {
                       T.speed=5;
                       T.TRAFFIC.y=400;
                       T.TRAFFIC.x=225;
                       break;
                       }
           }
}

void reposition_traffic_up(TRAFF &T, int j)
{
     int y=1;
     switch (j)
     {
            case 0:
                 {
                  if(T.TRAFFIC.y>775)
                  {
                                      T.TRAFFIC.y=0;
                                      switch (T.TRAFFIC.x)
                                      {
                                      case 40:
                                         {
                                          T.TRAFFIC.x=105;
                                          T.speed=2;
                                          break;
                                         }
                                      case 170:
                                         {
                                          T.TRAFFIC.x=40;
                                          T.speed=3;
                                          break;
                                         }
                                      case 105:
                                         {
                                          T.TRAFFIC.x=170;
                                          T.speed=2;
                                          break;
                                         }
                                      }
                  }
                  break;
                 }
            case 1:
                 {
                  if(T.TRAFFIC.y>810)
                  {
                                      T.TRAFFIC.y=0;
                                      switch (T.TRAFFIC.x)
                                      {
                                      case 40:
                                         {
                                          T.TRAFFIC.x=105;
                                          T.speed=2;
                                          break;
                                         }
                                      case 170:
                                         {
                                          T.TRAFFIC.x=40;
                                          T.speed=3;
                                          break;
                                         }
                                      case 105:
                                         {
                                          T.TRAFFIC.x=170;
                                          T.speed=2;
                                          break;
                                         }
                                      }                                    
                  }
                  break;
                 }
            case 2:
                 {
                  if(T.TRAFFIC.y>760)
                  {
                                      T.TRAFFIC.y=0;
                                      switch (T.TRAFFIC.x)
                                      {
                                      case 40:
                                         {
                                          T.TRAFFIC.x=170;
                                          T.speed=2;
                                          break;
                                         }
                                      case 170:
                                         {
                                          T.TRAFFIC.x=105;
                                          T.speed=2;
                                          break;
                                         }
                                      case 105:
                                         {
                                          T.TRAFFIC.x=40;
                                          T.speed=3;
                                          break;
                                         }
                                      }                                    
                  }
                  break;
                 }
            case 3:
                 {
                  if(T.TRAFFIC.y>800)
                  {
                                      T.TRAFFIC.y=0;
                                      switch (T.TRAFFIC.x)
                                      {
                                      case 40:
                                         {
                                          T.TRAFFIC.x=105;
                                          T.speed=2;
                                          break;
                                         }
                                      case 170:
                                         {
                                          T.TRAFFIC.x=40;
                                          T.speed=3;
                                          break;
                                         }
                                      case 105:
                                         {
                                          T.TRAFFIC.x=170;
                                          T.speed=2;
                                          break;
                                         }
                                      }                                    
                  }
                  break;
                 }
     }
     
}

void reposition_traffic_down(TRAFF &T, int j)
{
     switch (j)
     {
            case 0:
                 {
                  if(T.TRAFFIC.y>750)
                  {
                                    T.TRAFFIC.y=0;
                                    switch (T.TRAFFIC.x)
                                    {
                                    case 225:
                                         {
                                          T.TRAFFIC.x=355;
                                          T.speed=3;
                                          break;
                                         }
                                    case 355:
                                         {
                                          T.TRAFFIC.x=290;
                                          T.speed=4;
                                          break;
                                         }
                                    case 290:
                                         {
                                          T.TRAFFIC.x=225;
                                          T.speed=5;
                                          break;
                                         }
                                    }
                  }
                  break;
                 }
            case 1:
                 {
                  if(T.TRAFFIC.y>800)
                  {
                                    T.TRAFFIC.y=0;
                                    switch (T.TRAFFIC.x)
                                    {
                                    case 225:
                                         {
                                          T.TRAFFIC.x=290;
                                          T.speed=4;
                                          break;
                                         }
                                    case 355:
                                         {
                                          T.TRAFFIC.x=225;
                                          T.speed=5;
                                          break;
                                         }
                                    case 290:
                                         {
                                          T.TRAFFIC.x=355;
                                          T.speed=3;
                                          break;
                                         }
                                    }                                    
                  }
                  break;
                 }
            case 2:
                 {
                  if(T.TRAFFIC.y>785)
                  {
                                    T.TRAFFIC.y=0;
                                    switch (T.TRAFFIC.x)
                                    {
                                    case 225:
                                         {
                                          T.TRAFFIC.x=355;
                                          T.speed=3;
                                          break;
                                         }
                                    case 355:
                                         {
                                          T.TRAFFIC.x=290;
                                          T.speed=4;
                                          break;
                                         }
                                    case 290:
                                         {
                                          T.TRAFFIC.x=225;
                                          T.speed=5;
                                          break;
                                         }
                                    }                                    
                  }
                  break;
                 }
            case 3:
                 {
                  if(T.TRAFFIC.y>775)
                  {
                                    T.TRAFFIC.y=0;
                                    switch (T.TRAFFIC.x)
                                    {
                                    case 225:
                                         {
                                          T.TRAFFIC.x=290;
                                          T.speed=4;
                                          break;
                                         }
                                    case 355:
                                         {
                                          T.TRAFFIC.x=225;
                                          T.speed=5;
                                          break;
                                         }
                                    case 290:
                                         {
                                          T.TRAFFIC.x=355;
                                          T.speed=3;
                                          break;
                                         }
                                    }                                    
                  }
                  break;
                 }
     }
}

int check_position(TRAFF T)
{
    if(T.TRAFFIC.y<=85)
    {
     return 0;
    }
    else
    {
     return 1;
    }
}

int check4cop(SDL_Rect C)
{
     for(int i=0;i<4;i++)
     {
             if(T_up[i].TRAFFIC.y>=(C.y-83) && T_up[i].TRAFFIC.y<=(C.y+83))
             {
              return i;
             }
     }
}
void save_score(int s)
{
    int i;
    FILE *score_file;
    score_file=fopen("score.dat","w");
    for(i=0;i<10;i++)
    {
        if(s>scores[i])
        {
            scores[i]=s;
            break;
        }
    }
   
   for(i=0;i<10;i++)
    {
                     fprintf(score_file,"%d\n",scores[i]);
    }
    fclose(score_file);
}

void read_scores()
{
    int i;
    FILE*score_file;
    score_file=fopen("score.dat","r");
    
    for(i=0;i<10;i++)
    {
        fscanf(score_file,"%d",&scores[i]);
    }
    fclose(score_file);
}

void sort_scores()
{
    int i,j;
    int temp;
    for (i=0;i<10;i++)
    {
        for(j=i;j<10;j++)
        {
            if (scores[i]<scores[j])
            {
                temp=scores[i];
                scores[i]=scores[j];
                scores[j]=temp;
                }
            }
          }
     }
     
void display_scores()
{
    SDL_Rect temp;
    char abc[10];
    int i=0;
    
    SDL_Surface*back=NULL;
    SDL_Surface*message=NULL;
    TTF_Font *s;
    SDL_Color a={0,255,0};
    back=SDL_LoadBMP("images/back.bmp");
    back=SDL_DisplayFormat(back);
    SDL_BlitSurface(back,NULL,screen,NULL);
    s=TTF_OpenFont("font/digifaw.ttf",20);
    message=TTF_RenderText_Blended(s,"scores",a);
    temp.x=10;
    temp.y=10;
    SDL_BlitSurface(message,NULL,screen,&temp);
    for(i=0;i<10;i++)
    {
        itoa(scores[i],abc,10);
        temp.x=20;
        temp.y=30+i*20;
        message=TTF_RenderText_Blended(s,abc,a);
        SDL_BlitSurface(message,NULL,screen,&temp);
    }
    SDL_Flip(screen);
    SDL_Delay(2000);
    SDL_FreeSurface(back);
    SDL_FreeSurface(message);
    TTF_CloseFont(s);    
}
