#!/usr/bin/env python

import os
import socket
import re
import sys

def gen_svc_png(host, svc_infos): # list  svc_name:port
    template_cnt='''
    digraph G {
        rankdir=LR
        compound=true

        test [style=filled]
    '''

    #for ()
    # name = svc_info.name + svc_info.port
    # line = ""
    # template_cnt += ""
    #
    #

    dot_file_name = host + ".dot"
    png_file_name = host + ".png"

    open(dot_file_name, "w").write(tpt.substitute(kv))

    cmd = 'dot -Tpng %s -o %s'%(dot_file_name, png_file_name)
    os.system(cmd)

def check_server(address, port):
    #create a TCP socket
    s = socket.socket()
    s.settimeout(2)
    try:
        s.connect((address, port))
        return True
    except socket.error, e:
        return False
    finally:
        s.close()




def check_host_iptables(host, svc_infos):
    print "\nChecking " + host + " iptables svc now..... "
    print "---------------------------------------------"

    idx = 0
    for key in  sorted(svc_infos.keys()):
        idx = idx + 1
        if check_server(host, svc_infos[key]):
            print "[%03d] %20s [%d] Successful"%(idx, key, svc_infos[key])
        else:
            print "[%03d] %20s [%d] Failed" % (idx, key, svc_infos[key])


if __name__ == '__main__':
    svc_infos = {
        'activitysvc': 30012,
        'albumsvc': 30018,
        'appsearchsvc': 30022,
        'audiosvc': 30010,
        'chatletshopsvc': 30025,
        'codepushsvc': 30016,
        'commentsvc': 30024,
        'eventlogsvc': 30020,
        'friendsvc': 30001,
        'groupsvc': 30006,
        'im-connector-svc': 30011,
        'imagesvc': 30009,
        'mapsvc': 30008,
        'marketingsvc': 30028,
        'pagesvc': 30005,
        'passportsvc': 30003,
        'pomelohttpsvc': 30014,
        'postsvc': 30004,
        'recommendsvc': 30019,
        'reportsvc': 30026,
        'sensitivewordssvc': 30023,
        'statisticssvc': 30027,
        'tagsvc': 30017,
        'usersvc': 30007,
        'video-svc': 30013,
        'weathersvc': 30015

    }

    nodes = ['eengoo-node2',
     'eengoo-node3',
     'eengoo-node4',
     'eengoo-node5',
     'eengoo-node6',
     'eengoo-node7',
     'eengoo-node8',
     'eengoo-node9',
     'eengoo-node10',
     'eengoo-node36',
     'eengoo-node38',
     'eengoo-node37',
     'eengoo-node39',
     'eengoo-node41',
     'eengoo-node42']

    for host in nodes:
        check_host_iptables(host, svc_infos)
