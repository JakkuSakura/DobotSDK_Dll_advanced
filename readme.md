I modified this
```cpp
// in CDobot.cpp as well as in CDobot.h
CDobot *CDobot::instance()
{
    static CDobot *instance[64] = {0};
    int dobotId = getDobotID();
    if (instance[dobotId] == 0) {
        instance[dobotId] = new CDobot();
    }
    return instance[dobotId];
}
static int currentId = 0;
int CDobot::getDobotID()
{
    return currentId;
}
void CDobot::specifyDobotID(int id)
{
    currentId = id;
}

````
and here
```cpp
// in DobotDll.cpp as well as in DobotDll.h
int GetDobotID(void)
{
    return CDobot::getDobotID();
}

void SpecifyDobotID(int dobotId)
{
    CDobot::specifyDobotID(dobotId);
}

int DobotExec(void)
{
    CDobot::instance()->exec();

    return 0;
}


```