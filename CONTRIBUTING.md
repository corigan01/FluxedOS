# How to contribute
* Make a fork of the project
* Commit fairly (use details of what you changed in each commit)
* Submit a pull request for addition to the main branch
* Check changes

## How to fork
* Click the fork button at the top right corner
![](https://docs.github.com/assets/images/help/repository/fork_button.jpg)
* If you need more help, click [here](https://docs.github.com/en/github/getting-started-with-github/fork-a-repo)


## What is a fair commit
After you make a change, make a small explanation of the changes you made. An example is 
```bash
&> git add *
&> git commit -a

Added 'MyNewFile.txt'. This file helps explain the contributing. 

# Please enter the commit message for your changes. Lines starting
# with '#' will be ignored, and an empty message aborts the commit.
#
# On branch main
# Your branch is up to date with 'origin/main'.
#
# Changes to be committed:
#       new file:   MyNewFile.txt
#
# Untracked files:
#       .vscode/
#
# Press ctrl+X then Y then enter to exit and save

&> git push
```

## Make a pull request
Go to the main project source (corigan01/FluxedOS). Then click on pull requests. From here you pull in your repo into the main branch. 

## Check changes
When you make a pull-request github will check to see if the code works. If github fails to build the code, then you will need to fix and commit those changes. 
If github is able to build the file, then we will pull it into the main branch!

