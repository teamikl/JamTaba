#include "Server.h"
#include "User.h"
#include <QDebug>

using namespace Ninjam;

QMap<QString, std::shared_ptr<Server>> Server::servers;

Server::Server(QString host, int port)
    :port(port), host(host),
      maxUsers(0), bpm(120), bpi(16),
      activeServer(true),
      streamUrl(""),
      topic(""),
      containBot(false)
{

    //qDebug() << "criou server " << host <<":" << port;
}


QString Server::getUniqueName(QString host, int port) {
    return host + ":" + port;
}

bool Server::containsUser(const User &user) const
{
    return users.contains(user.getFullName());
}


Server* Server::getServer(QString host, int port) {
    QString key = getUniqueName(host, port);
    if (!servers.contains(key)) {
        servers.insert(key, std::shared_ptr<Server>(new Server(host, port)));
    }
    return servers[key].get();
}

void Server::addUser(User* user) {
    if (!users.contains(user->getFullName())) {
        users.insert(user->getFullName(), user);
        if (user->isBot()) {
            containBot = true;
        }
    }
}

QList<User*> Server::getUsers() const{
    return users.values();
}

bool Server::containsBotOnly() const{
    if (users.size() == 1 && containBot) {
        return true;
    }
    return false;
}

QString Server::getUniqueName() const
{
     return host + ":" + port;
}

bool Server::setBpm(short bpm) {
    if (bpm == this->bpm) {
        return false;
    }

    if (bpm >= Server::MIN_BPM && bpm <= Server::MAX_BPM) {
        this->bpm = bpm;
        return true;
    }
    return false;
}

bool Server::setBpi(short bpi) {
    if (bpi == this->bpi) {
        return false;
    }

    if (bpi >= Server::MIN_BPI && bpi <= Server::MAX_BPI) {
        this->bpi = bpi;
        return true;
    }
    return false;
}

void Server::refreshUserList(QSet<User*> onlineUsers) {
    QList<User*> toRemove;

    foreach (User* onlineUser , onlineUsers) {
        addUser(onlineUser);
    }

    QList<User*> currentUsers= users.values();
    foreach (User* user , currentUsers) {
        if (!onlineUsers.contains(user)) {
            toRemove.append(user);
        }
    }

    foreach (User* ninjaMUser , toRemove) {
        users.remove(ninjaMUser->getFullName());
    }

}

QDataStream & Ninjam::operator<<(QDataStream &out, const Server &server){
    out << "NinjamServer{" << "port="  <<  server.getPort()  <<  ", host="
        <<  server.getHostName() <<  ", stream="  <<  server.getStreamUrl()
        <<" maxUsers="  <<  server.getMaxUsers() <<  ", bpm="
        <<  server.getBpm()  <<  ", bpi="  <<  server.getBpi()
        <<  ", isActive="  <<  server.isActive() <<  "}\n";
    for (User* user : server.getUsers()) {
        out << "\t" << user->getName()  << "\n";
    }
    return out;
}