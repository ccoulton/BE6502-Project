#ifndef _IRC_BOT_H_
#define _IRC_BOT_H_

extern CRGB led;
void connectToKnownNetworks(void);
void IrcBotTask(void* parameters);

#endif