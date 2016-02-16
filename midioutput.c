#include <alsa/asoundlib.h>
#include <wiringPi.h>
#include <limits.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

static snd_seq_t *seq_handle;
static int in_port;

////////////////////////////////////////////////////////////////////////////
//Example setup: There are 12 melody channels. Each index is mapped to 
// the corresponding Wiring Pi valued channel in the array below.
//
//////////////////////////////////////////////////////////////////

// start alternate keymapping at 53
int KeyMapping[] = {1,512,2,513,4,514,8,16,515,32,516,64,128};
int pinMapping[] = {25,21,0,23,14,2,24,30,12,22,13,3,   15,8,9,7,16,1,4,5};
//int pinMapping[] = {0,2,3,12,13,14,21,30,22,23,24,25,15,8,9,7,16,1,4,5};

int PositionArray[][3] ={{0,0,0},{1,1,0},{1,0,1},{0,1,0},
			 {0,0,0},{1,1,0},{1,0,1},{0,1,0},
			{0,0,0},{1,1,0},{1,0,1},{0,1,0},
			{0,0,0},{1,1,0},{1,0,1},{0,1,0}};


int Status=0;
int CStat[]={0,0,0,0};
int CStatBuf[]={0,0,0,0};
int pinUsed[]={0,0,0,0,0,0,0,0,0,0,0,0};

#define TOTAL_PINS sizeof(pinMapping) / sizeof(int)
#define THRUPORTCLIENT 14
#define THRUPORTPORT 0

void midi_open(void)
{
    snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_INPUT, 0);

    snd_seq_set_client_name(seq_handle, "LightOrgan");
    in_port = snd_seq_create_simple_port(seq_handle, "listen:in",
                      SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
                      SND_SEQ_PORT_TYPE_APPLICATION);
 
    if( snd_seq_connect_from(seq_handle, in_port, THRUPORTCLIENT, THRUPORTPORT) == -1) {
       perror("Can't connect to thru port");
       exit(-1);
    } 

}


snd_seq_event_t *midi_read(void)
{
    snd_seq_event_t *ev = NULL;
    snd_seq_event_input(seq_handle, &ev);
    return ev;
}


//Currently playing note, by pin
int pinNotes[TOTAL_PINS];

//Currently playing channel, by pin
int pinChannels[TOTAL_PINS];

//Enabled channels
int playChannels[16];


void clearPinNotes() {
   int i;
   for(i=0; i< TOTAL_PINS; i++) {
      pinNotes[i] = -1;
   }
}

int alternateKey(int pinIdx){
	if((pinIdx >= 53) && (pinIdx <= 65))
		return KeyMapping[pinIdx-53];
	else return pinIdx < 65 ? pinIdx + 512-53 : pinIdx + 512-61;
}

void myDigitalWrite(int pinIdx, int val) {
	printf("Alt-key = %d\n", alternateKey(pinIdx));
     val  ?  printf("%i (%i) ON\n", pinIdx, pinMapping[pinIdx])  : printf("%i (%i) OFF\n", pinIdx, pinMapping[pinIdx]);
     digitalWrite( pinMapping[pinIdx], val );

}

void WriteIOBuf(int Idx, int Offset){
//	printf(" idx = %d, Offset = %d ", Idx, Offset);
//	printf(" pin = %d, %d, %d ", pinMapping[Offset*3], pinMapping[Offset*3+1],pinMapping[Offset*3+2]);
//	printf(" pin = %d %d %d ", PositionArray[Offset*4+Idx][0],PositionArray[Offset*4+Idx][1],PositionArray[Offset*4+Idx][2]);
	digitalWrite( pinMapping[Offset*3], 0 );
	pinUsed[Offset*3] = PositionArray[Offset*4+Idx][0];
	digitalWrite( pinMapping[Offset*3], PositionArray[Offset*4+Idx][0] );
	digitalWrite( pinMapping[Offset*3+1], 0 );
	pinUsed[Offset*3+1] = PositionArray[Offset*4+Idx][1];
	digitalWrite( pinMapping[Offset*3+1], PositionArray[Offset*4+Idx][1] );
	digitalWrite( pinMapping[Offset*3+2], 0 );
	pinUsed[Offset*3+2] = PositionArray[Offset*4+Idx][2];
	digitalWrite( pinMapping[Offset*3+2], PositionArray[Offset*4+Idx][2] );


}

void clearPinChannels() {
   int i;
   for(i=0; i< TOTAL_PINS; i++) {
      pinChannels[i] = INT_MAX;
   }
}

void clearPinsState() {
  clearPinNotes();
  clearPinChannels();
}

void pinsOn() {
   int i;
   for(i=0; i< TOTAL_PINS; i++) { 
      myDigitalWrite(i, 1); 
   }
}

void pinsOff() {
   int i;
   for(i=0; i< TOTAL_PINS; i++) {
      myDigitalWrite(i, 1); 
   }
}


void setChannelInstrument(int channel, int instr) {
  printf("setting channel %i to instrument %i\n", channel, instr);
  playChannels[channel] = instr;  
}



int isPercussion(int instrVal) {
  return instrVal >= 8 && instrVal <= 15;
}

int isPercussionChannel(int channel) {
  int instr = playChannels[channel];
  return isPercussion(instr);
}


int isBase(int instrVal) {
  return instrVal >= 32 && instrVal <= 39;
}
int isSynth(int instrVal) {
  return instrVal >= 88 && instrVal <= 103;
}

int choosePinIdx(int note, int channel) {
   //Return the note modulated by the number of melody pins
//   int val = note  % (TOTAL_PINS * 2);
//   return val / 2;
	return note % 19;
}



void midi_process(snd_seq_event_t *ev)
{
    
    //If this event is a PGMCHANGE type, it's a request to map a channel to an instrument
    if( ev->type == SND_SEQ_EVENT_PGMCHANGE )  {
       //printf("PGMCHANGE: channel %2d, %5d, %5d\n", ev->data.control.channel, ev->data.control.param,  ev->data.control.value);

       //Clear pins state, this is probably the beginning of a new song
       clearPinsState();
       
       setChannelInstrument(ev->data.control.channel, ev->data.control.value);
    }

    //Note on/off event
    else if ( ((ev->type == SND_SEQ_EVENT_NOTEON)||(ev->type == SND_SEQ_EVENT_NOTEOFF)) ) {
        
  
        //choose the output pin based on the pitch of the note
        int pinIdx = alternateKey(ev->data.note.note);
		int RedIdx=(pinIdx-6)%12;
		//choosePinIdx(ev->data.note.note, ev->data.note.channel);
		printf("Note=%d", ev->data.note.note);
		int isOn = 1;
		if( ev->data.note.velocity == 0 || ev->type == SND_SEQ_EVENT_NOTEOFF) {
              isOn = 0;
           }
		

		
		if( isOn ) {
			if(pinIdx != 523){
				if(pinIdx < 256) {
				CStat[0]+=pinIdx & 0x3;
				CStat[1]+=(pinIdx & 12) >> 2;
				CStat[2]+=(pinIdx & 48) >> 4;
				CStat[3]+=(pinIdx & 196) >> 6;}
				else{
					if(!pinUsed[RedIdx])
					myDigitalWrite(RedIdx, 1);
					else myDigitalWrite(RedIdx, 0);
					
				printf(" On %d",pinIdx < 256 ? pinIdx*1000: pinIdx);
				}
				
			}
			else {
				system("/home/pi/midioutput/play.sh");
				// reageer op key 523
			}
			}
		else {
				if(pinIdx < 256) {
				CStat[0]-=pinIdx & 0x3;
				CStat[1]-=(pinIdx & 12) >> 2;
				CStat[2]-=(pinIdx & 48) >> 4;
				CStat[3]-=(pinIdx & 196) >> 6;}
				else{
					
					if(!pinUsed[RedIdx])
					myDigitalWrite(RedIdx, 0);
					else myDigitalWrite(RedIdx, 1);
				}

				printf(" Off %d",pinIdx < 256 ? pinIdx*1000: pinIdx);
			}

		
		if(CStat[0]!=CStatBuf[0]) {CStatBuf[0]=CStat[0];  WriteIOBuf(CStat[0], 0);}
		if(CStat[1]!=CStatBuf[1]) {CStatBuf[1]=CStat[1];  WriteIOBuf(CStat[1], 1);}
		if(CStat[2]!=CStatBuf[2]) {CStatBuf[2]=CStat[2];  WriteIOBuf(CStat[2], 2);}
		if(CStat[3]!=CStatBuf[3]) {CStatBuf[3]=CStat[3];  WriteIOBuf(CStat[3], 3);}

printf("\n");
    }
    
    else {
      //rintf("Unhandled event %2d\n", ev->type);
   
    }

    snd_seq_free_event(ev);
}


int main()
{
    //Setup wiringPi
    if( wiringPiSetup() == -1) {
      exit(1);
    }   
    //Setup all the pins to use OUTPUT mode
    int i=0;
    for(i=0; i< TOTAL_PINS; i++) {
      pinMode( pinMapping[i], OUTPUT);
    }


    clearPinsState();

    //Open a midi port, connect to thru port also
    midi_open();

    //Process events forever
    while (1) {
       midi_process(midi_read());
    }

    return -1;
}
