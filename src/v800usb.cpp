/*
    Copyright 2014 Christian Weber

    This file is part of V800 Downloader.

    V800 Downloader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    V800 Downloader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with V800 Downloader.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "v800usb.h"
#include "native_usb.h"
#include <stdio.h>

V800usb::V800usb(int device) {
    usb = NULL;
    this->device = device;
}

V800usb::~V800usb() {
    if(usb != NULL) {
        usb->close_usb();
        delete usb;
    }
}

int V800usb::start() {
    int ret = -1;
    usb = new native_usb();

    if(device == V800)
        ret = usb->open_usb(0x0da4, 0x0008);
    else if(device == M400)
        ret = usb->open_usb(0x0da4, 0x0008);

    return (ret != -1);
}

/* void V800usb::get_sessions(QList<QString> sessions)
{
    QString session;
    QStringList session_split;
    QList<QString> multi_sports, files, temp_session_files, temp_files;
    QDateTime session_time;
    int session_iter, files_iter, multi_sports_iter, multi_sport_cnt;

    for(session_iter = 0; session_iter < sessions.length(); session_iter++)
    {
        session_time = QDateTime::fromString(sessions.at(session_iter), Qt::TextDate);
        session = session_time.toString(tr("yyyyMMdd/HHmmss"));
        session_split = session.split(tr("/"));

        if(session_split.length() == 2)
        {
            multi_sports.clear();
            multi_sports = get_v800_data(QString(tr("%1/%2/E/%3/")).arg(tr(V800_ROOT_DIR)).arg(session_split[0]).arg(session_split[1]));

            multi_sport_cnt = 0;

            for(multi_sports_iter = 0; multi_sports_iter < multi_sports.length(); multi_sports_iter++)
            {
                if(multi_sports[multi_sports_iter].contains(tr("/")))
                {
                    files.clear();
                    files = get_v800_data(QString(tr("%1/%2/E/%3/%4/")).arg(tr(V800_ROOT_DIR)).arg(session_split[0]).arg(session_split[1]).arg(multi_sports[multi_sports_iter]));

                    for(files_iter = 0; files_iter < files.length(); files_iter++)
                    {
                        temp_files = get_v800_data(QString(tr("%1/%2/E/%3/%4/%5")).arg(tr(V800_ROOT_DIR)).arg(session_split[0]).arg(session_split[1]).arg(multi_sports[multi_sports_iter]).arg(files[files_iter]), multi_sport_cnt);
                        if(temp_files.length() == 1)
                            temp_session_files.append(temp_files[0]);
                    }

                    temp_files = get_v800_data(QString(tr("%1/%2/E/%3/TSESS.BPB")).arg(tr(V800_ROOT_DIR)).arg(session_split[0]).arg(session_split[1]), multi_sport_cnt);
                    if(temp_files.length() == 1)
                        temp_session_files.append(temp_files[0]);

                    temp_files = get_v800_data(QString(tr("%1/%2/E/%3/PHYSDATA.BPB")).arg(tr(V800_ROOT_DIR)).arg(session_split[0]).arg(session_split[1]), multi_sport_cnt);
                    if(temp_files.length() == 1)
                        temp_session_files.append(temp_files[0]);

                    multi_sport_cnt++;
                }
            }

            QString tag = QDateTime(QDate::fromString(session_split[0], tr("yyyyMMdd")), QTime::fromString(session_split[1], tr("HHmmss"))).toString(tr("yyyyMMddhhmmss"));
            emit session_done(tag, session_iter, sessions.length());
        }
    }

    emit sessions_done();
}
*/

/* void V800usb::get_all_objects(QString path)
{
    QList<QString> objects;

    objects = get_v800_data(path);
    emit all_objects(objects);
}
*/

void V800usb::get_file(std::string path) {
    get_v800_data(path, true);
}

/* void V800usb::upload_route(QString route)
{
    qDebug("Route file: %s", route.toLatin1().constData());

    if(!QFile(QString(tr("%1/PROUTE.BPB")).arg(route)).exists())
    {
        qDebug("No PROUTE.BPB");
        return;
    }

    if(!QFile(QString(tr("%1/ID.BPB")).arg(route)).exists())
    {
        qDebug("No ID.BPB");
        return;
    }

    if(!QFile(QString(tr("%1/TST.BPB")).arg(route)).exists())
    {
        qDebug("No TST.BPB");
        return;
    }

    QList<QString> favs, routes;
    int max_fav_num = 0, max_route_num = 0, new_route_num;

    favs = get_v800_data(tr("/U/0/FAV/"));
    routes = get_v800_data(tr("/U/0/ROUTES/"));

    for(int cnt = 0; cnt < favs.length(); cnt++)
    {
        if(max_fav_num < favs[cnt].remove(tr("/")).toInt())
            max_fav_num = favs[cnt].remove(tr("/")).toInt();
    }

    for(int cnt = 0; cnt < routes.length(); cnt++)
    {
        if(max_route_num < routes[cnt].remove(tr("/")).toInt())
            max_route_num = routes[cnt].remove(tr("/")).toInt();
    }

    if(favs.length() == 0)
        max_fav_num = -1;
    if(routes.length() == 0)
        max_route_num = -1;

    if(max_fav_num != max_route_num)
    {
        qDebug("Unbalanced route and favorites, can't add route :(");
//        return;
    }

    new_route_num = max_route_num+1;
    qDebug("New route and fav: %d", new_route_num);

    if(new_route_num > 99)
    {
        qDebug("More than 99 routes, can't add another");
        return;
    }

    new_route_num = 0;

    put_v800_dir(tr("/U/0/ROUTES/"));
    put_v800_dir(QString(tr("/U/0/ROUTES/%1/")).arg(new_route_num));
    put_v800_data(QString(tr("%1/PROUTE.BPB")).arg(route), QString(tr("/U/0/ROUTES/%1/PROUTE.BPB")).arg(new_route_num));

    remove_v800_dir(tr("/U/0/FAV"));
    put_v800_dir(tr("/U/0/FAV/"));
    put_v800_dir(QString(tr("/U/0/FAV/%1/")).arg(new_route_num, 2, 10, QChar(0x30)));
    put_v800_data(QString(tr("%1/ID.BPB")).arg(route), QString(tr("/U/0/FAV/%1/ID.BPB")).arg(new_route_num, 2, 10, QChar(0x30)));
    put_v800_data(QString(tr("%1/TST.BPB")).arg(route), QString(tr("/U/0/FAV/%1/TST.BPB")).arg(new_route_num, 2, 10, QChar(0x30)));
}
*/

/* void V800usb::get_all_sessions()
{
    QList<QString> dates, times, files, sessions;
    int dates_iter, times_iter, files_iter;
    bool session_exists = false;

    dates = get_v800_data(QString(tr("%1/")).arg(tr(V800_ROOT_DIR)));
    for(dates_iter = 0; dates_iter < dates.length(); dates_iter++)
    {
        times.clear();
        if(dates[dates_iter].contains(tr("/")))
        {
            times = get_v800_data(QString(tr("%1/%2E/")).arg(tr(V800_ROOT_DIR)).arg(dates[dates_iter]));

            for(times_iter = 0; times_iter < times.length(); times_iter++)
            {
                files.clear();
                files = get_v800_data(QString(tr("%1/%2/E/%3/00/")).arg(tr(V800_ROOT_DIR)).arg(dates[dates_iter]).arg(times[times_iter]));

                for(files_iter = 0; files_iter < files.length(); files_iter++)
                {
                    if(QString(files[files_iter]).compare(tr("SAMPLES.GZB")) == 0)
                    {
                        session_exists = true;
                        break;
                    }
                }

                if(session_exists)
                {
                    QString date(dates[dates_iter]);
                    QString time(times[times_iter]);

                    QString combined((QString(tr("%1%2")).arg(date).arg(time)));
                    QDateTime session_time = QDateTime::fromString(combined, tr("yyyyMMdd/HHmmss/"));

                    sessions.append(session_time.toString(Qt::TextDate));
                    session_exists = false;
                }
            }
        }
    }

    emit all_sessions(sessions);
}
*/

int V800usb::get_v800_data(std::string request, bool debug) {
    QByteArray packet, full;
    int cont = 1, usb_state = 0, packet_num = 0, initial_packet = 1;

    while(cont) {
        // usb state machine for reading
        switch(usb_state) {
        case 0: // send a command to the watch
            packet.clear();
            packet = generate_request(request);

            usb->write_usb(packet);

            packet_num = 0;
            usb_state = 1;
            break;
        case 1: // we want to read the buffer now
            packet.clear();
            packet = usb->read_usb();

            // check for end of buffer
            if(is_end(packet)) {
                full = add_to_full(packet, full, initial_packet, true);
                usb_state = 4;
            } else {
                full = add_to_full(packet, full, initial_packet, false);
                usb_state = 2;
            }

            // initial packet seems to always have two extra bytes in the front, 0x00 0x00
            if(initial_packet)
                initial_packet = false;
            break;
        case 2: // send an ack packet
            packet.clear();
            packet = generate_ack(packet_num);
            if(packet_num == 0xff)
                packet_num = 0x00;
            else
                packet_num++;

            usb->write_usb(packet);

            usb_state = 1;
            break;
        case 4:
            if(!debug) 
				fprintf(stderr, "only debug mode is currently supported\n");
            else {
				full.write(stdout);
                /*if(request.contains(tr("."))) {
                    request.replace(tr("/"), tr("_"));

                    QSettings settings;
                    QString file_dir = settings.value(tr("file_dir")).toString();

                    QFile *debug_file;
                    debug_file = new QFile(QString(tr("%1/%2")).arg(file_dir).arg(request));
                    debug_file->open(QIODevice::WriteOnly);
                    debug_file->write(full);
                    debug_file->close();
                }*/
            }

            cont = 0;
            break;
        }
    }

    return (cont != 0);
}

/* QList<QString> V800usb::extract_dir_and_files(QByteArray full)
{
    QList<QString> dir_and_files;
    int full_state = 0, size = 0, loc = 0;

    while(loc < full.length())
    {
        switch(full_state)
        {
        case 0: // look for 0x0A
            if(full[loc] == (char)0x0A)
            {
                loc++;
                full_state = 1;
            }

            loc++;
            break;
        case 1: // is this the second 0x0A?
            if(full[loc] == (char)0x0A)
                full_state = 2;
            else
                full_state = 0;

            loc++;
            break;
        case 2: // get the size
            size = full[loc];

            full_state = 3;
            loc++;
            break;
        case 3: // we need a 0x10 after the string
            if(full.at(loc+size) == 0x10)
                full_state = 4;
            else
                full_state = 0;
            break;
        case 4: // now get the full string
            QString name(tr(QByteArray(full.constData()+loc, size)));

            dir_and_files.append(name);

            full_state = 0;
            loc += size;
            break;
        }
    }

    return dir_and_files;
}
*/

QByteArray V800usb::generate_request(std::string request) {
    QByteArray packet;

    packet[0] = 01;
    packet[1] = (unsigned char)((request.length()+8) << 2);
    packet[2] = 0x00;
    packet[3] = request.length()+4;
    packet[4] = 0x00;
    packet[5] = 0x08;
    packet[6] = 0x00;
    packet[7] = 0x12;
    packet[8] = request.length();
    packet.append((uint8_t*)request.c_str(), request.length());

    return packet;
}

QByteArray V800usb::generate_ack(unsigned char packet_num) {
    QByteArray packet;

    packet[0] = 0x01;
    packet[1] = 0x05;
    packet[2] = packet_num;

    return packet;
}

int V800usb::is_end(QByteArray packet) {
    return ((packet[1] & 0x03) != 1);
}

QByteArray V800usb::add_to_full(QByteArray packet, QByteArray full, bool initial_packet, bool final_packet) {
    QByteArray new_full = full;
    unsigned int size = (unsigned char)packet[1] >> 2;
	unsigned int start = 3;

	size--; //drop first byte
    if(initial_packet) {
		// initial packets have an extra two leading bytes
		size -= 2;
        start += 2;
    }
    // final packets have a trailing 0x00 we don't want
//TODO is this trailing 0x00 because it's a Cstring?
	if(final_packet)
		size--;

	packet.remove(0, start);
	if(size > packet.length()) {
		fprintf(stderr, "warning, packet is shorter than it should be!\n");
		size = packet.length();
	}
	new_full.append((uint8_t*)packet.constData(), size);

    return new_full;
}

/* void V800usb::remove_v800_dir(QString dest)
{
    qDebug("Dir: %s", dest.toLatin1().constData());

    QByteArray packet;
    int cont = 1, usb_state = 0;

    while(cont)
    {
        // usb state machine for writing
        switch(usb_state)
        {
        case 0: // send the initial packet to the watch
            packet.clear();
            packet = generate_directory_command(dest.toUtf8(), true);
            usb->write_usb(packet);

            usb_state = 1;

            break;
        case 1: // see what we got from the v800
            packet.clear();
            packet = usb->read_usb();

            qDebug("Done creating directory");
            cont = 0;

            break;
        }
    }
}
*/

/* void V800usb::put_v800_dir(QString dest)
{
    qDebug("Dir: %s", dest.toLatin1().constData());

    QByteArray packet;
    int cont = 1, usb_state = 0;

    while(cont)
    {
        // usb state machine for writing
        switch(usb_state)
        {
        case 0: // send the initial packet to the watch
            packet.clear();
            packet = generate_directory_command(dest.toUtf8(), false);
            usb->write_usb(packet);

            usb_state = 1;

            break;
        case 1: // see what we got from the v800
            packet.clear();
            packet = usb->read_usb();

            qDebug("Done creating directory");
            cont = 0;

            break;
        }
    }
}
*/

/* void V800usb::put_v800_data(QString src, QString dest)
{
    qDebug("Src: %s\nDest: %s", src.toLatin1().constData(), dest.toLatin1().constData());

    QFile src_file(src);
    QByteArray packet, full, data;
    int cont = 1, usb_state = 0, packet_num, data_loc;

    src_file.open(QIODevice::ReadOnly);
    full = src_file.readAll();

    while(cont)
    {
        // usb state machine for writing
        switch(usb_state)
        {
        case 0: // send the initial packet to the watch
            data = full.mid(0, 55-dest.length());

            packet.clear();
            packet = generate_initial_command(dest.toUtf8(), data);
            usb->write_usb(packet);

            data_loc = 55-dest.length();
            packet_num = 1;
            usb_state = 1;

            break;
        case 1: // see what we got from the v800
            packet.clear();
            packet = usb->read_usb();

            if(!is_command_end(packet))
                usb_state = 2;
            else
                usb_state = 3;

            break;
        case 2:
            data = full.mid(data_loc, 61);

            packet.clear();
            packet = generate_next_command(data, packet_num);
            usb->write_usb(packet);

            data_loc += 61;
            if(packet_num == 0xff)
                packet_num = 0x00;
            else
                packet_num++;

            usb_state = 1;

            break;
        case 3:
            qDebug("Done with file");

            cont = 0;
            break;
        }
    }
}
*/

/* QByteArray V800usb::generate_directory_command(QByteArray dest, bool remove)
{
    QByteArray packet;

    packet[0] = 01;
    packet[1] = (unsigned char)((dest.length()+8) << 2);
    packet[2] = 0x00;
    packet[3] = dest.length()+4;
    packet[4] = 0x00;
    packet[5] = 0x08;
    if(remove)
        packet[6] = 0x03;
    else
        packet[6] = 0x01;
    packet[7] = 0x12;
    packet[8] = dest.length();
    packet.append(dest);

    return packet;
}
*/

/* QByteArray V800usb::generate_initial_command(QByteArray dest, QByteArray data)
{
    QByteArray packet;

    packet[0] = 01;
    packet[1] = (unsigned char)((dest.length()+data.length()+7) << 2);
    packet[2] = 0x00;
    packet[3] = dest.length()+4;
    packet[4] = 0x00;
    packet[5] = 0x08;
    packet[6] = 0x01;
    packet[7] = 0x12;
    packet[8] = dest.length();
    packet.append(dest);
    packet.append(data);

    if(packet.length() == 64)
        packet[1] = packet[1] | 0x01;

    return packet;
}
*/

/* QByteArray V800usb::generate_next_command(QByteArray data, int packet_num)
{
    QByteArray packet;

    packet[0] = 01;
    packet[1] = (unsigned char)((data.length()+1) << 2);
    packet[2] = packet_num;
    packet.append(data);

    if(packet.length() == 64)
        packet[1] = packet[1] | 0x01;

    return packet;
}
*/

/* int V800usb::is_command_end(QByteArray packet)
{
    if(packet.at(1) == 0x10)
        return 1;
    else
        return 0;
}
*/
