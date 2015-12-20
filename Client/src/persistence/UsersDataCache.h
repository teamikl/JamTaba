#ifndef USERSDATACACHE_H
#define USERSDATACACHE_H


#include <QString>
#include <QMap>
#include <QRegExp>

/***
  This class is used to store/remember the users level, pan, mute and boost. When a user enter in the jam
  the data is recovered/remembered from this cache.
 */

namespace Persistence {

#define DEFAULT_MUTE false
#define DEFAULT_GAIN 1.0f
#define DEFAULT_PAN 0.0f
#define DEFAULT_BOOST 1.0f
#define PAN_MAX 4.0f
#define PAN_MIN -4.0f

class CacheEntry{//cache entries are per channel, not per user.
public:
    CacheEntry(const QString& userIp, const QString& userName, quint8 channelID);
    CacheEntry(){}

    inline bool isMuted() const{ return muted; }
    inline float getGain() const{ return gain; }
    inline float getPan() const{ return pan; }
    inline float getBoost() const{ return boost; }
    inline QString getUserIP() const{ return userIp; }
    inline QString getUserName() const{ return userName; }
    inline quint8 getChannelID() const{ return channelID; }

    void setUserIP(const QString& userIp);
    void setUserName(const QString& userName);
    void setChannelID(quint8 channelID);
    void setMuted(bool muted);
    void setPan(float pan);
    void setBoost(float boost);
    void setGain(float gain);

    static QRegExp ipPattern;
    static QRegExp namePattern;
private:
    QString userIp;
    QString userName;
    quint8 channelID;
    bool muted;
    float gain;//fader level
    float pan;
    float boost;
};


//++++++++++++++++++++++++++++++++
class UsersDataCache
{
public:
    UsersDataCache();
    ~UsersDataCache();
    CacheEntry getUserCacheEntry(QString userIp, QString userName, quint8 channelID);//return default values for pan, gain and mute if user is not cached yet
    void updateUserCacheEntry(CacheEntry entry);
private:
    QMap<QString, CacheEntry> cacheEntries;

    static QString getUserUniqueKey(const QString& userIp, const QString& userName, quint8 channelID);

    void loadCacheEntriesFromFile();
    void writeCacheEntriesToFile();

    const QString CACHE_FILE_NAME;

};

}//namespace

#endif // USERSDATACACHE_H
