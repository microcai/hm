#hm - A Command Line Hotel Management Tool

hm usese command syntax very simalier to git.

## To create a new hotel database

run

	hm init --skel

and edit skel/skel_room.txt

then rerun

	hm init [roomlist]

this command can be run multible times.

## To add client info
run

	hm client add

and fill in the info in the openning editor. you can set your editor by $EDITOR env

## To modify client info
run

	hm client mod [somerefs]

somerefs is a piese if infomation that can locate the client. Say, nick, phone number ID etc

## To book the rooms
run

	hm book	[roomlist] by [somerefs] at  [date]

or (this syntax not implemented)
	
	hm book	[roomlist] by [somerefs] from [date] to [date]


## To check free rooms
run

	hm status [date]

or (TO-BE done)

	hm list free [date]


## To check free rooms with PC ( not avaliable now)
run

	hm list free [date] with PC

note that  [DBDIR]/rooms/roomid/info should have equipment=PC line






