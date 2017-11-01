# Old Amiga Stuffs

Some of these things were put on Aminet. Many are unfinished
experiments, tests made to learn, finished or unfinished utilities,
translations from higher level languages (like AmigaE, C) of
educational (sometimes also useful) code, "remakes" of programs
written by others (but their programs were *bigger*, not *pure*, not
written in “100% assembly” — bad thing for me in that age :-D),
implementations of something I read on some magazine, or alike.

Few files were edited or removed.

Executables aren't included, but I want to show the list of a LHA
archive to show their sizes (second column).

    [generic]                  212  85.4% Jun 17  1997 c/AskErr
    [generic]                  312  74.7% Apr 11  1996 c/ColdReboot
    [generic]                  984  72.4% Jun 17  1997 c/Device
    [generic]                  756  70.4% Mar  7  1996 c/DFree
    [generic]                  704  71.7% Jun 17  1997 c/FreeX
    [generic]                  172  98.8% Jun 17  1997 c/pwd
    [generic]                 1108  62.9% Jul 21  1998 c/ReadDevice
    [generic]                  412  76.5% Apr 11  1996 c/Reboot
    [generic]                  448  88.2% Jul 20  1998 c/RequestChoice
    [generic]                  996  69.8% Oct 21  1998 c/RequestFile
    [generic]                  428  85.0% Jul 23  1998 c/RequestModeID
    [generic]                  568  82.9% Jul 20  1998 c/RequestString
    [generic]                  748  78.7% Jun 17  1997 c/XCom

(The `c` drawer was like `bin` on GNU/Linux or similar systems.)

All files `.a` are 68k assembly sources: I preferred `.a` instead of
`.asm` or the very common and usual `.s`. I used to use GenAm as
assembler.

In source files you can find comments in Italian and English; the
encoding for non ASCII letters, when used, is Latin1 (which was the
default on Amiga, AFAIR).

All email addresses you can find here and there are dead and must not
be used — if they still exist and someone answers, **it's not me**!

Some sources appeared more than once in different “collections”,
without any tiny modification; hence I've deleted duplicates,
identified by computing the MD5 sum of the files. If you find holes in
references, you know you can fill them searching for the file in
another subtree. This [list](MD5.list) might help you: it was done
before deleting duplicates.


## License

Often in the sources or docs it is stated that the programs are
*freeware*, with a stress on the fact that they cost nothing, and must
not cost anything nor be used for commercial purposes.

Also, few comments make me think that by *freeware* I meant something
that can be also modified as long as my name and other names, if they
appeared, were kept.

This sounds like an attribution clause.

In `ReadMe.First` you can read:

    All the files in the archive are declared as Freeware or, if
     specified, Public Domain (PD).
    
    All the sources, though not very clear, can be used for all the
    purpose you can imagine, but ONLY FREE (no money, zero-cost).
    
    You can use them in your Freeware or PD sources/programs,
    not in Shareware and not in Commercial Products.
    
    (For Giftware and all those forms which have not a payment in money
    the Sources can be used; or for all the software for which there is
    a VOLUNTARY payment)

Of course by *voluntary payment* I intended *donations*!

It seems like in `cluts_mra` I had a different idea (grammar errors
included, e.g., *this* instead of *these*):

      All the programs are freely distributable (FREEWARE). This means that you can get
      a single program and re-distribute it, WITH ITS SOURCE in assembly language,
      if given, and ITS OWN DOCUMENTATION. Only If you keep altogether this three things
      you can re-distribute single program (the archive is a collection of several
      programs). You can re-distribute also the whole archive, of course.
     *
      You ARE NOT ALLOWED to modify the program(s) in anyway, even if the source in
      assembly language is given. These sources are given only for study and analysis.
      Only UN-MODIFIED program (include: its source, its documentation) can be
      re-distribute.

I think it's ok to consider the less restrictive license you can
deduce from what I've written. Anyway, I don't think correct licensing
will be an issue anymore.


