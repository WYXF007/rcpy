#!/usr/bin/env python3
# import python libraries
import time
import getopt, sys

# import rcpy library
# This automatically initizalizes the robotics cape
import rcpy 
import rcpy.servo as servo
import rcpy.clock as clock

def usage():
    print("""usage: python rcpy_test_servos [options] ...
Options are:
-d duty     define a duty cycle from -1.5 to 1.5
-c servo    specify a single servo from 1-8, 0 for all servos
-s          sweep servos back and forward at duty cycle
-h          print this help message""")

def main():

    # exit if no options
    if len(sys.argv) < 2:
        usage()
        sys.exit(2)
    
    # Parse command line
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hsd:c:", ["help"])

    except getopt.GetoptError as err:
        # print help information and exit:
        print('rcpy_test_servos: illegal option {}'.format(sys.argv[1:]))
        usage()
        sys.exit(2)

    # defaults
    duty = 0.0
    channel = 0
    sweep = False
    brk = False
    free = False

    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in "-d":
            duty = float(a)
        elif o in "-c":
            channel = int(a)
        elif o == "-s":
            sweep = True
        else:
            assert False, "Unhandled option"

    # set state to rcpy.RUNNING
    rcpy.set_state(rcpy.RUNNING)

    # set servo duty (only one option at a time)
    srvo = servo.Servo(channel)
    if duty != 0:
        if not sweep:
            print('Setting servo {} to {} duty'.format(channel, duty))
            srvo.set(duty)
        else:
            print('Sweeping servo {} to {} duty'.format(channel, duty))
    else:
        sweep = False

    # message
    print("Press Ctrl-C to exit")

    clock = clock.Clock(srvo, 0.02)
    
    try:

        # enable servos
        servo.enable()
        
        # start clock
        clock.start()
        
        # sweep
        if sweep:

            d = 0
            direction = 1
            delta = duty/20
            
            # keep running
            while rcpy.get_state() != rcpy.EXITING:

                # running
                if rcpy.get_state() == rcpy.RUNNING:

                    # increment duty
                    d = d + direction * delta
                    srvo.set(d)

                    # end of range?
                    if d > duty or d < -duty:
                        direction = direction * -1
                        
                # sleep some
                time.sleep(.1)

        # or do nothing
        else:

            # keep running
            while rcpy.get_state() != rcpy.EXITING:
                
                # sleep some
                time.sleep(1)
            
    except KeyboardInterrupt:
        # handle what to do when Ctrl-C was pressed
        pass
        
    finally:

        # stop clock
        clock.stop()
        
        # disable servos
        servo.disable()
        
        # say bye
        print("\nBye Beaglebone!")
            
# exiting program will automatically clean up cape

if __name__ == "__main__":
    main()
