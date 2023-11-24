transform left:
    xalign 0.25

transform right:
    xalign 0.75

default love_confess = False

transform alpha_dissolve:
    alpha 0.0
    linear 0.5 alpha 1.0
    on hide:
        linear 0.5 alpha 0

screen countdown:
    timer 0.01 repeat True action If(time > 0, true=SetVariable('time', time - 0.01), false=[Hide('countdown'), Jump(timer_jump)])
    bar value time range timer_range xalign 0.5 yalign 0.9 xmaximum 300 at alpha_dissolve

init:
    $ timer_range = 0
    $ timer_jump = 0


define k = Character("Keiko", who_color="#FFFF00")
define t = Character("Taishiro")

label start:

    stop music
    $ pn = renpy.input("Please enter your name")
    $ pn = pn.strip()
    if pn == "":
        $ pn = "Lazy Player Level 9001"


    scene apartment b living room day
    with fade
    show taishiro schoolsummer happy
    with dissolve
    t "Howdy %(pn)s"
    pn "Wait, did you just break into my house?"
    show taishiro schoolsummer angry
    play music "audio/piano_bgm.mp3"
    t "Does it even matter?"
    t "We are running late. Let's go now!"
    pn "Hey we still have 1 hour left to get to school"
    t "Did you forget? It's %(k)s's birthday today"

    menu:
        "Choose any one of the options"
        "Tell the truth that you forgot":
            jump truth
        "Just lie and hope he is dumb enough":
            jump lie

    label truth:

        stop music
        play sound "audio/svn.mp3"
        pn "Oh dang it, it's today? I totally forgot about it"
        show taishiro schoolsummer normal
        t "Hey no worries. Just wear your uniform and let's go"
        jump school

    label lie:

        stop music
        pn "Hey hey, you think I would forget about her birthday?"
        pn "We have plenty of time. Chill man"
        show taishiro schoolsummer surprised
        t "We do?"
        pn "Ya sure. Let me get ready and then we'll go"
        jump school

    label school:

        scene classroom a st2 day
        with fade
        ""
        show taishiro schoolsummer normal
        with dissolve
        t "Okay, let's leave our bags here and go to the roof"
        pn "I'll carry the cake"
        pn "Let's go"

        scene school a s2st2 day
        with fade
        show taishiro schoolsummer normal
        t "Okay, I've already texted her to arrive early today and meet us at the roof"
        pn "Nicely done"

        show taishiro schoolsummer happy at right
        show keiko schoolsummer normal at left
        with dissolve
        k "Hey what are you gu-"
        t "HAPPY BIRTHDAAAAY"
        pn "Happy birthday %(k)s!"

        show keiko schoolsummer happy at left
        k "Aww you guys!"
        k "So that's why %(t)s asked me to arrive here early today"

        t "We even got you a cake"
        pn "Here it is, blow the candles and make a wish"

        show cake1:
            xalign 0.5
            yalign 0.5

        k "I sure will"
        k "Fuuuuu"

        hide cake1
        show cake2:
            xalign 0.5
            yalign 0.5

        show taishiro schoolsummer normal at right
        show keiko schoolsummer pout at left

        k "I hope you guys brought some nice gifts for me"

        hide cake2

        "%(pn)s, what will you do? Will you be courageous and give her the gift or will you shy away?"

        menu:
            "Give her the gift":
                jump gift
            "Tell her you did not bring any gifts":
                jump no_gifts

    label gift:

        pn "Ahem"
        pn "I got you this"

        show taishiro schoolsummer surprised at right
        show keiko schoolsummer blush at left

        k "These are so pretty"
        k "Thank you so much"

        $love_confess = True

        pn "Let's go somewhere nice after school"

        show taishiro schoolsummer normal at right
        show keiko schoolsummer normal at left

        k "I'll head home and change first though"
        pn "Okay sure"

        jump meetup


    label no_gifts:

        pn "Sorry, we didn't bring anything else other than this cake"

        show taishiro schoolsummer sad at right
        show keiko schoolsummer sad at left

        k "Oh..."
        k "Well that's okay"

        pn "Hey we can go somewhere nice after school"

        show taishiro schoolsummer normal at right
        show keiko schoolsummer normal at left

        k "Ya okay, that sounds fun but I'll first go home and change"
        t "Okay we will pick you up later then"

        jump meetup

    label meetup:

        scene house a evening
        ""
        show keiko casual normal
        with dissolve
        pn "Heyo"
        pn "%(t)s will be a little late. He said he has some work at their shop"
        k "No worries"
        k "Can we umm...Go to the nearby neighbourhood park first?"
        pn "Yes sure. You have the right to make all the calls today"
        show keiko casual happy
        k "Thank you"
        show keiko casual normal
        k "Let's go"

        scene park s1 evening
        with fade
        show keiko casual blush
        with dissolve
        k "I actually have something important to tell you"
        pn "What is it?"
        k "I-I..."
        k "I've fallen in love with you..."
        show keiko casual sad
        k "And I wish you would feel the same"
        k "But seeing how you just avoid me"
        k "I guess that's not the case"

        menu:
            "Remember every decisions have consequences"
            "Speechless":
                "You decided to remain silent"
                jump speechless
            "Confess" if love_confess:
                jump tell_her_already_bro

    label speechless:

        show taishiro beachwear happy at left
        show keiko casual surprised at right

        pn "What the heck is this man?"
        t "You guys!"
        t "A customer just gave us a free trip to an island resort"
        k "But why are you already wearing your beachwear"
        show taishiro beachwear surprised at left
        t "Umm...I don't know..."
        show taishiro beachwear happy at left
        t "Anyways, my parents are super busy."
        t "So, I have two extra slots. Let's goooooo"
        show keiko casual normal at right
        k "Sure. I need a break anyway"
        pn "Okay..."

        scene island resort s1 day with fade
        ""
        show taishiro beachwear happy at left
        show keiko dress pout at right
        t "Here we are!"
        pn "Keiko?"
        k "Remember, I always pout at beaches"
        pn "Why?"
        k "No reason"
        t "Let's play!"
        k "Yes!"

        scene forest s1 nightlights
        with fade
        pn "Uhh..."
        pn "Where are we?"
        show taishiro outing sad at left
        show keiko dress sad at right
        t "Yo man, you can't ask but you gotta save one of us"
        pn "What?"
        k "Yes, you can save only one of us"
        t "Save me man, you don't love her anyway"
        pn "WHAT DID YOU SAY!?"

        "Hurry now, you don't have much time"

        label slow:
            $ time = 3
            $ timer_range = 3
            $ timer_jump = 'too_slow'
            show screen countdown
            menu:
                "Just beat the heck out of %(t)s yourself":
                    hide screen countdown
                    jump ouch
                "Pick up %(k)s and run for it":
                    hide screen countdown
                    jump run

    label too_slow:

        "You were too slow in deciding what to do"
        hide taishiro outing sad
        hide keiko dress sad
        "Both %(k)s and %(t)s just left you all alone in this scary forest"
        "BAD ENDING"

        return

    label ouch:

        hide taishiro
        show keiko dress angry

        "You beat the crap out of %(t)s"
        "%(k)s calls the security"
        "And you end up in prison"

        "This is how you become a prisoner ending"

        return

    label run:

        scene hotel room b day
        "You grab %(k)s and run for it"
        "You both end up back in your hotel room"
        "And somehow it's daytime now"
        show keiko dress blush
        k "Actually %(t)s told me all about it"
        k "About how I was wrong and that you actually love me aswell"
        pn "I always did but just couldn't confess"
        pn "I love you %(k)s"
        k "I love you too, %(pn)s"

        "Happy Ending Part 2"

        return

    label tell_her_already_bro:

        pn "It's not that I avoid you because I don't feel the same"
        show keiko casual surprised
        pn "It's because I'm very shy"
        pn "And I'm shy around you the most because I actually feel the same"
        pn "Keiko!"
        show keiko casual blush
        k "Yes?"
        pn "I love you!"
        k "I love you too"

        scene house a nightlights
        with fade
        show keiko casual blush
        with dissolve
        pn "So, I'll pick you up tomorrow"
        k "Yes, Good night %(pn)s"
        pn "Good night"

        hide keiko casual blush
        with dissolve

        "Happy Ending"

        return
