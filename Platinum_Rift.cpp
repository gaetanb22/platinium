#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <vector>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

int getRand2()
{
    struct timeval tp;
    usleep(4);
    gettimeofday(&tp, NULL);
    return (tp.tv_usec);
}

int getRand(int nb)
{
    int div = 1;

    while (nb / div >= 10)
    {
        div*=10;
    }

    int nbTotal = 0;
    bool status = false;
    int mod;
    int n;

    while (div != 0)
    {
        if (status == true)
            mod = 9;
        else
            mod = (nb / div) % 10;
        n = getRand2() % (mod + 1);
        if (n == mod)
            status = true;
        nbTotal += n * div;
        div /= 10;
    }
    return (nbTotal);
}

class zone
{
public:
    int zoneId;
    int platinumSource;
    std::list<zone*> listLink;

    int ownerId;
    int pods[4];
    int purchase;

    int distanceFromFrontiers;


    int valueZone;
    int nbDiv;
    int valueArmy;
    int nbDivArmy;
    int getNbAdjacentEnemies(int myId)
    {
        int nb = 0;
        for (std::list<zone*>::iterator it=listLink.begin(); it != listLink.end(); ++it)
        {
            nb += pods[0] + pods[1] + pods[2] + pods[3] - pods[myId];
        }
        return (nb);
    }
    void initValueZone(int currentValue = -1)
    {
        if (currentValue == -1)
            currentValue = platinumSource / 10;

        valueZone += currentValue;//((valueZone * nbDiv) + currentValue) / (nbDiv + 1);
        nbDiv++;

        for (std::list<zone*>::iterator it=listLink.begin(); it != listLink.end(); ++it)
        {
            if ((*it)->nbDiv != nbDiv)
                (*it)->initValueZone(currentValue / 2);
        }
    }
    void initValueArmy(int myId, bool status = false, int currentValue = -1000)
    {
        if (status == false)
            currentValue = (pods[myId] * 2 - pods[0] - pods[1] - pods[2] - pods[3]) * 1000;
        valueArmy += currentValue;
        nbDivArmy++;

        for (std::list<zone*>::iterator it=listLink.begin(); it != listLink.end(); ++it)
        {
            if ((*it)->nbDivArmy != nbDivArmy)
                (*it)->initValueArmy(myId, true, currentValue / 2);
        }
    }
    bool isAtFrontier()
    {
        for (std::list<zone*>::iterator it=listLink.begin(); it != listLink.end(); ++it)
        {
            if ((*it)->ownerId != ownerId)
            {
                return true;
            }
        }
        return false;
    }
    zone(int _zoneId, int _platinumSource)
    {
        zoneId = _zoneId;
        platinumSource = _platinumSource * 1000;
        purchase = 0;
        valueZone = 0;
        nbDiv = 0;
    }
    void addLink(zone *_link)
    {
        listLink.push_back(_link);
    }
    void refresh()
    {
        int zId;
        cin >> zId >> ownerId >> pods[0] >> pods[1] >> pods[2] >> pods[3]; cin.ignore();
    }
    int getPodsOwner()
    {
        return (pods[ownerId]);
    }
    void setDistanceFromFrontiers2(int depth, int depthMax, bool &status)
    {
        if (depth >= depthMax)
            return;
        for (std::list<zone*>::iterator it=listLink.begin(); it != listLink.end(); ++it)
        {
            if ((*it)->ownerId == ownerId && (distanceFromFrontiers + 1 <= (*it)->distanceFromFrontiers || (*it)->distanceFromFrontiers == -1))
            {
                if (depth + 1 == depthMax && (*it)->distanceFromFrontiers != 0)
                {
                    if ((*it)->isAtFrontier())
                        (*it)->distanceFromFrontiers = 0;
                    (*it)->distanceFromFrontiers = depth + 1;
                    status = true;
                }
                else
                {
                    (*it)->setDistanceFromFrontiers2(depth + 1, depthMax, status);
                }
            }
        }
    }
    void setDistanceFromFrontiers()
    {
        int depthMax;
        bool status = true;

        distanceFromFrontiers = 0;
        depthMax = distanceFromFrontiers + 1;
        while (status)
        {
            status = false;
            setDistanceFromFrontiers2(distanceFromFrontiers, depthMax, status);
            depthMax++;
        }

    }
};

class map
{
public:
    int myPlatinium;
    int playerCount;
    int myId;
    int zoneCount;
    int linkCount;
    std::vector<zone> listZone;
    map()
    {
        cin >> playerCount >> myId >> zoneCount >> linkCount; cin.ignore();
        for (int i = 0; i < zoneCount; i++)
        {
            int zoneId;
            int platinumSource;
            cin >> zoneId >> platinumSource; cin.ignore();
            listZone.push_back(zone(zoneId, platinumSource));
        }
        for (int i = 0; i < linkCount; i++)
        {
            int zone1;
            int zone2;
            cin >> zone1 >> zone2; cin.ignore();
            listZone[zone1].addLink(&listZone[zone2]);
            listZone[zone2].addLink(&listZone[zone1]);
        }
        initValueZone();
    }
    void purchasePod(std::stringstream &cmd)
    {
        int nb = 0;
        for (int count = 0; count < zoneCount; count++)
        {
            if (listZone[count].distanceFromFrontiers == 0 || listZone[count].ownerId == -1)
            {
                nb+=((listZone[count].platinumSource / 500) + 1);
            }
        }
        if (nb != 0)
        {
            int nZone = getRand(nb);
            nb = 0;
            for (int count = 0; count < zoneCount; count++)
            {
                if (listZone[count].distanceFromFrontiers == 0 || listZone[count].ownerId == -1)
                {
                    nb+=((listZone[count].platinumSource / 500) + 1);
                    if (nb > nZone)
                    {
                        listZone[count].purchase++;
                        return;
                    }
                }
            }
        }
        for (int i = 0; i < zoneCount; i++)
        {
            if (listZone[i].ownerId == myId || listZone[i].ownerId == -1)
            {
                listZone[i].purchase++;
                return;
            }
        }
    }
    void purchase()
    {
        std::stringstream cmd;

        while (myPlatinium >= 20)
        {
            purchasePod(cmd);
            myPlatinium-=20;
        }
        for (int i = 0; i < zoneCount; i++)
        {
            if (listZone[i].purchase > 0)
            {
                if (cmd.str().size() > 0)
                    cmd << ' ';
                cmd << listZone[i].purchase << ' ' << listZone[i].zoneId;
                listZone[i].purchase = 0;
            }
        }
        if (cmd.str().size() == 0)
        {
            std::cout << "WAIT" << std::endl;
        }
        else
        {
            std::string str = cmd.str();
            std::cout << str << std::endl;
        }
    }
    void mooveZone(zone &currentZone, std::stringstream &cmd, bool status = false)
    {
        zone *ptr = NULL;
        for (std::list<zone*>::iterator it=currentZone.listLink.begin(); it != currentZone.listLink.end(); ++it)
        {
            if ((*it)->ownerId != myId
                && currentZone.getPodsOwner() + (*it)->pods[myId] > (*it)->getPodsOwner() + (*it)->getNbAdjacentEnemies(myId)
                && (status == true || (*it)->pods[myId] <= (*it)->getPodsOwner()))
            {
                if (ptr == NULL || (*it)->platinumSource > ptr->platinumSource)
                    ptr = (*it);
            }
        }
        if (ptr != NULL && currentZone.zoneId != ptr->zoneId)
        {
            int nb;
            if (status == false)
                nb = ptr->getPodsOwner() - ptr->pods[myId] + 1;
            else
                nb = currentZone.getPodsOwner();
            if (currentZone.platinumSource > ptr->platinumSource)
                nb -= currentZone.getNbAdjacentEnemies(myId);
            if (nb <= 0)
                return ;
            if (cmd.str().size() > 0)
                cmd << ' ';
            cmd << nb << ' ' << currentZone.zoneId << ' ' << ptr->zoneId;
            ptr->pods[myId] += nb;
            currentZone.pods[myId] -= nb;
            return;
        }
        if (currentZone.distanceFromFrontiers == 0)
            return;
        for (std::list<zone*>::iterator it=currentZone.listLink.begin(); it != currentZone.listLink.end(); ++it)
        {
            if ((*it)->distanceFromFrontiers < currentZone.distanceFromFrontiers && currentZone.zoneId != (*it)->zoneId)
            {
                if (cmd.str().size() > 0)
                    cmd << ' ';
                cmd << currentZone.getPodsOwner() << ' ' << currentZone.zoneId << ' ' << (*it)->zoneId;
                return;
            }
        }
    }
    void moove()
    {
        std::stringstream cmd;
        for (int i = 0; i < zoneCount; i++)
        {
            if (listZone[i].ownerId == myId && listZone[i].pods[myId] > 0)
            {
                int nbPods = -1;
                if (listZone[i].distanceFromFrontiers != 0)
                    mooveZone(listZone[i], cmd);
                else
                {
                    while (listZone[i].pods[myId] != 0 && nbPods != listZone[i].pods[myId])
                    {
                        nbPods = listZone[i].pods[myId];
                        mooveZone(listZone[i], cmd);
                    }
                    if (listZone[i].pods[myId] > 0)
                    {
                        mooveZone(listZone[i], cmd, true);
                    }
                }
            }
        }
        if (cmd.str().size() == 0)
        {
            std::cout << "WAIT" << std::endl;
        }
        else
        {
            std::string str = cmd.str();
            std::cout << str << std::endl;
        }
    }

    void refresh()
    {
        int platinum;
        cin >> platinum; cin.ignore();
        myPlatinium = platinum;
        for (int i = 0; i < zoneCount; i++)
        {
            listZone[i].refresh();
        }
    }
    void refreshArmyValue()
    {
        for (int i = 0; i < zoneCount; i++)
        {
            listZone[i].valueArmy = 0;
            listZone[i].nbDivArmy = 0;
        }
        for (int i = 0; i < zoneCount; i++)
        {
            listZone[i].initValueArmy(myId);
        }
    }
    void refreshFrontier()
    {
        for (int i = 0; i < zoneCount; i++)
        {
            listZone[i].distanceFromFrontiers = -1;
        }
        for (int i = 0; i < zoneCount; i++)
        {
            if (listZone[i].ownerId == myId && listZone[i].isAtFrontier() == true)
            {
                listZone[i].setDistanceFromFrontiers();
            }
        }
    }
    void initValueZone()
    {
        for (int i = 0; i < zoneCount; i++)
        {
            if (listZone[i].platinumSource != 0)
                listZone[i].initValueZone();
        }
    }
    void play()
    {
        refresh();
        refreshFrontier();
        refreshArmyValue();
        moove();
        purchase();
    }
};

int main()
{
    map *currentMap = new map();
    while (1)
        currentMap->play();
}