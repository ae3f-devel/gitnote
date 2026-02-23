```
             _____  __         _ _               _   _             
   __ _  ___|___ / / _| _ __ _(_) |_ _ __   ___ | |_(_) ___  _ __  
  / _` |/ _ \ |_ \| |(_|_) _` | | __| '_ \ / _ \| __| |/ _ \| '_ \ 
 | (_| |  __/___) |  _| | (_| | | |_| | | | (_) | |_| | (_) | | | |
  \__,_|\___|____/|_|(_|_)__, |_|\__|_| |_|\___/ \__|_|\___/|_| |_|
                         |___/
```
> syncs the repository to [Notion](https://www.notion.com) pages.   
> tracks the changes from file and syncronise it on call.

# dependencies
- git binary
- python binary and the dependencies
    > not specified yet

- cmake with build tools (c)

# to use this programme you need:
- the [git repository](https://github.com/git/git) you want to link
- the [Notion api key](https://www.notion.so/profile/integrations)
- the [page id to use as root](https://developers.notion.com/reference/retrieve-a-page)

# what will happen
- the `page id` you've fed to this programme will be directory page.
- inside this, the repository will be cloned to the remote (notion)
