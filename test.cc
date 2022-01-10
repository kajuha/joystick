// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright Drew Noakes 2013-2016

#include "joystick.hh"
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>

int main(int argc, char** argv)
{
  // Create an instance of Joystick
  Joystick joystick("/dev/input/js0");

  // Ensure that it was found and that we can use it
  if (!joystick.isFound())
  {
    printf("open failed.\n");
    exit(1);
  }

	char name[128];
  const char oga_name[] = "GO-Advance Gamepad (rev 1.1)";
	if (ioctl(joystick.getFD(), JSIOCGNAME(sizeof(name)), name) < 0)
		strncpy(name, "Unknown", sizeof(name));
	printf("Name: %s\n", name);

  if (strcmp(name, oga_name)) {
    printf("go advance gamepad : connection failed\n");
    printf("connected gamepad : %s\n", name);

    return -1;
  }

#define D_SIZE 18
#define A_SIZE 2
#define BTN_SIZE (D_SIZE+A_SIZE)
  int buttons[BTN_SIZE];

  for (int i=0; i<BTN_SIZE; i++) {
    buttons[i] = -99999;
  }

  while (true)
  {
    // Restrict rate
    usleep(1000);

    // Attempt to sample an event from the joystick
    JoystickEvent event;
    if (joystick.sample(&event))
    {
      if (event.isButton())
      {
        // printf("Button %u is %s\n",
        //   event.number,
        //   event.value == 0 ? "up" : "down");
        buttons[event.number] = event.value;
#define BUTTON_X 2
#define BUTTON_PUSHED 1
        if (event.number == BUTTON_X && event.value == BUTTON_PUSHED) {
          return 0;
        }
      }
      else if (event.isAxis())
      {
        buttons[D_SIZE+event.number] = event.value;
        // printf("Axis %u is at position %d\n", event.number, event.value);
      }

      if (!event.isInitialState()) {
        for (int i=0; i<D_SIZE; i++) {
          // printf("[%d]", buttons[i]);
          printf("%d", buttons[i]);
        }
        for (int i=0; i<A_SIZE; i++) {
          printf("[%+06d]", buttons[D_SIZE+i]);
        }
        printf("\n");
      }
    }
  }
}
