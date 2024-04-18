//#include "fmod.h"
#include "mod_sound.h"
BG_Music::BG_Music()
{

}
BG_Music::~BG_Music()
{
	// nothing to destroy
}

bool BG_Music::OpenFile(char * filename, vector<string> &file_list)
{
	const int MAX_LEN = 260;

	FILE *file = fopen(filename, "r");
	if(!file)
		return false;
	char song_name[MAX_LEN];
	char ch = 0;
	int i = 0;
	bool not_eof = true;

	while(not_eof)
	{
		i = 0;
		while( (ch = fgetc(file)) != '\n')
		{
			if(ch == EOF)
				break;
			song_name[i++] = ch;
		}
		song_name[i] = 0;
		if(ch == EOF)
		{
			not_eof = false;
			break;
		}
		string str = song_name;
		file_list.push_back(str);		
	}
	fclose(file);
	return true;
}
bool BG_Music::Init()
{
	// All initialization code goes here
	FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);

	FSOUND_SetDriver(0);

	// initialize here

	if(!FSOUND_Init(44100, 32, 0))
	{
		printf("Error in init()\n");
		FSOUND_Close();
		return 1;
	}
	return true;
}

bool BG_Music::Play(const char * filename, bool repeat)
{
	Stream = FSOUND_Stream_OpenFile(filename, FSOUND_NORMAL | FSOUND_LOOP_NORMAL, 0);
	if(!Stream)
	{
		printf("Error in stream\n");
		//FSOUND_Close();
		return false;
	}
	
	channel = FSOUND_Stream_PlayEx(FSOUND_FREE, Stream, NULL, true);
	FSOUND_SetPaused(channel, false);	
	return true;
}

bool BG_Music::Stop()
{
	if(!FSOUND_Stream_Close(Stream))
	{
		printf("Error, couldn't close stream!\n");
		printf("%d\n", Stream);
		return false;
	}
	else
		return true;
}

void BG_Music::Pause(bool pause)		
{
	if(pause)
	{
		FSOUND_SetPaused(channel, true);
	}
	else
		FSOUND_SetPaused(channel, false);
}

void BG_Music::Destroy()
{
	FSOUND_Close();
}

/************************************************************************************

  Sound Effects

*************************************************************************************/

bool Effect::PlayEffect(char * filename, int channel)
{
	if(!filename)				// just in case some loon decided to pass a null filename
		return false;

	Sptr = FSOUND_Sample_Load(/*FSOUND_FREE*/channel, filename, FSOUND_LOOP_OFF | FSOUND_2D, 0);
	if(!Sptr)
		return false;
	else
		FSOUND_PlaySound(FSOUND_FREE, Sptr);
	
	return true;
}

void Effect::StopEffect()
{
//	FSOUND_StopSound(Sptr);
}

Effect::Effect()
{

}

Effect::~Effect()
{

}