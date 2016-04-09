Software analysis:
1.Requirment: 
 a.The program will create a volumn of appropriate size(about two time of the dir) and store the
   dir.
 b.When a volumn number is specified, and store the directory within. When not
   enough space is left. And display an error.
 c.Error and info are directed to respective channel.
 d.The program provide two ways of backup: direct writing and filesystem sync
 e.The program outputs a volumn-id where directory is stored(or the specified
   one is echoed)
 f.When environment variable EC2_BACKUP_VERBOSE is set, the program print info
   on each step it's undertaking.
 g.When environment variable EC2_BACKUP_FLAGS_AWS is set, the value string is
   examined and intelligently added to the instance creation command
 h.The program checks variable EC2_BACKUP_FLAGS_SSH in which user can specifiy
   identify file for ssh connection.
 i.The program will not set any environment variable or create any file for
   temporary use.(Thus require the environment being set up to some extent)

Mechanism:
1.Transfer of data: the program will create an instance and optionally a volumn,
attatch the volumn, exchange data with proper command.



2.Possible errors and handling:
 a.volumn specified but not enough space are left
 ## Most erros may arise during instance creation
 b.


Problem:
1.What if the identity file is protected by a passphrase, how to relay the prompt and response?
2.What if AWS FLAG include overlapping options? Replace? Report? Error?
3.What kind of storage type am I choosing.
4.What aws instance creation command am I expecting?
