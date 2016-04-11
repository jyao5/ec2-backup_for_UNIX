SOFTWARE ANALYSIS:
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

POSSIBLE PROBLEMS:
1.What if the identity file is protected by a passphrase, how to relay the prompt and response?
2.What if AWS FLAG include overlapping options? Replace? Report? Error?
3.What kind of storage type am I choosing.
4.What aws instance creation command am I expecting?


ABOUT INSTANCE CREATION AND VOLUME ATTACHMENT
1.if no security group is specified, a default one is used 
  A default security group:
  Allow inbound traffic only from other instances associated with the default
  security group.
  Allow all outbound traffic from the instance.

  Since the program is expected to access the instance via ssh. Thus the user
  must either enable SSH connection in default security group or specify a
  group with SSH enable. (May be the program can temporarily create a group for
  operation. Technically this doesn't change user's preset environment)

2.To log in to the instance, a key pair must be used, thus be specified when the
  instance was created. As elucidated in the manual, when not specifically
  specified, the program will first search in ~/.ssh/ for valid private/public
  key pair for creation of and accessing to the temporary instance. 
  When an identity file is specified in the environment variable, the program
  is supposed to create/access the instance using the this file. Note: only
  private key is need and public will be generated using ssh(1) and imported to
  aws.
  When aforementioned methods fail, an message of attempt is displayed, and the
  program try to generate an temporary key(in /tmp) for use. The key will be
  output to the terminal(for, not violating user's "environment"). Then the key
  will be destructed.
  If all methods don't work, an error is displayed.

  More thought:
  The program is not allowed to set any environment variable(hence I suppose
  create any file as well)
  The program assumes the user has set up the ~/.ssh/config file to access
  instances in EC2 via ssh(1). This implies that ssh will not prompt password
  for authentication which is not a simple task to provide by its
  invoker(ec2-backup) through process communication api. Hence the
  program assume ssh will not perform interactive prompt of any kind for 
  passkey. Thus if a key is protected by a passphrase, the ec2-backup will
  assume that a ssh-agent is running and passphrase is already added.
  If variable is set, an additional option\pointer to specified identity file
  location is append to ssh.
  So the problem is to find what identity file is to be used to connect instance
  via ssh. Location of the indentity file must be used to identify which key to
  use to run the instance. Personal opinion, what's the point of specify
  identity file for ssh connection as long as the instance is temporary. The key
  can also be temporary. Unless this is to conform the "no environment
  violation" rule. If this is true, then it make sense.

  CONCLUSION: The program must find a private key of some kind provided by the user(it
  can't generate its own private key--violate local host environment, and upload it,
  even if just temporarily, to aws ec2). Then it should use the key to somehow
  find corresponding key name in aws, which has also been set up by the user.
  The use this key name to run the instance.

3.For the sake of not incurring any unexpected expense to the user. An instance
  type--t1.micro is explicitly specified(whereas the default one is m1.small)
  And user is not supposed to specified instance type
  The program is supposed to maintain a list of workable instance image ids. I
  sense there should be some kind of way to update the image id. Or we could
  automate the process of acquiring an appropriate ami.


Steps:
1). Search for identity files for ssh access. Use the identity files to find
   keynames.If no identity files were found, report error.(it's nearly not
   implementable to parse ~/.ssh/config for identity file name, since temporary
   instance IP is dynamicly allocated. Thus the program only check default key name under
   .ssh/ or use name offered in the variable)

   ls ~/.ssh/ | grep '^id_rsa$' #and for other default key name
   if echo $? is 0, use the key to find respective key name in 
     describe-key- pair 
   Otherwise check if a file is provided by the variable for use.
   Otherwise display error and exit(>=1)

    openssl pkey -in ~/.ssh/id_rsa -pubout -outform DSA | openssl sha1 -c
    openssl pkcs8 -in aws_private.pem -nocrypt -topk8 -outform DER | openssl
    sha1 -c
    from:Daniel
    https://forums.aws.amazon.com/thread.jspa?messageID=386670&tstart=0

    Then match it to describe-key-pair and find key name

    A much more easy way is to generate public key and imported it for temporary
    use
    In this case if an identity file is found. We use ssh-keygen to get it's
    public key and imported the key.


1.1) if volunm number is given, check availability zone of the volumn, if region
    don't match, prompt the user to change region(error)
    If two matches, create the instance according to the subnet id of the zone.
    If no vol num is specified, create the instance and volumn with the first
    zone found in the output of describe-availability-zone.
2).construct commands, append EC2_BACKUP_FLAGS_AWS to default
   commands.(the program only allows instance type overwrite, key-name is
   determined by default or by location in EC2_BACKUP_FLAGS_SSH. Security group
   names are allowed for accessibility, image id are not allowed for remote
   command execution is OS dependent, and the program can't handle operations
   in arbitrary OS.
3).Run the instances
4).Attatch the volumn. If a volumn ID is specified. Zone availability may need extra handling.
5).send command to instance to mount and backup the directory


FACTS LEARNED
1.argument passing
argument are passed through main(int argc, and char **argv)
argc(short for argument counter) stands for the number of argument passed to the
main(include the funciton name).
argv is a list of pointer to each argument(as a string), note that argv[0] stores the
function name.( Thus valid argument range: 0 ~ (argc-1) )

2.environment variable extraction
Programs executed from the shell inherit all of the environment variables from
the shell.
In c, char* [secure_]getenv(const char* name) can be used to extract content
from environment variable. Note, "NAME" return a char pointer to the string.
The user is responsible to export the variable so as to make it available to
chile process. The program will deem monitored variable not set if it's null,
but still give suggestion in verbose mode(provided VERBOSE are set)
Note: this function is declared in the stdlib.h. Thus a check on whether
this function is supported may be performed before hand.

3.strcmp takes two cstring(pointer may be) as arguments and output 0 if these
strings match, return value >0 if fisrt unmatch charater has higher value in
ptr1 than in ptr2. Similar situation for return value <0.

4.return is an instruction that return from a function call
  exit is a system call that terminates the current process

  In main(), two makes no difference
  If in a function, exit() teminate the processes

5.int system(string) is a function that executes system command. Output is direct to
  standard output, return value corresponds to exit status of the command.

  popen, pclose: pipe stream to or from a process
  FILE *popen(const char *command, const char *type);
  int pclose(FILE *stream);
  Pipe is UNIdirectional, thus type is either read-only or write-only
  Command is a null-terminated string passed to /bin/sh using -c flag(read from
  a string instead of standard input)
  Return NULL when error happened
  Popen only capture standard output. Use redirection to get/subdue error
  message. We can redirect stdin and stderr to different file for further
  processing.

  fgets to extract the output of the commandi into a cstring, line by line.
  char *fgets(char *s, int size, FILE *stream);// store size-1 charactes into
  buffer s, or stop at a newline or EOF, '\0' is appended(that's why buffer of
  size 'size' store only size-1 characters. Return s on success, NULL on error
  or end encountered.

