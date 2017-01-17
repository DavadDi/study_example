#!/usr/bin/env python
# coding=utf-8

from string import Template
import os
import json
import socket
import re
import sys
import time
import httplib


def check_server(address, port):
    #create a TCP socket
    s = socket.socket()
    s.settimeout(2)
    try:
        s.connect((address, port))
        return True
    except socket.error, e:
        return False

def gen_svc_png_template(svc_name, kv):
    template_cnt='''
    digraph G {
        rankdir=LR
        compound=true

        nginx[style=filled]

        subgraph cluster0 {
                node [style=filled];
                style=solid;

                "$node1";
                "$node2";
                label="$cluster_ip";
        }

        nginx -> "$node1" [color=$n1_color];
        nginx -> "$node2" [color=$n1_color];

        "$endpoint1"[style=filled]
        "$endpoint2"[style=filled]

        "$node1" ->  "$endpoint1" [color=$ep1_color]
        "$node1" ->  "$endpoint2"[color=$ep2_color]

        "$node2" ->  "$endpoint1" [color=$ep1_color]
        "$node2" ->  "$endpoint2"[color=$ep2_color]
    }
    '''

    tpt = Template(template_cnt)

    """
    kv = { 'node1': 'node35_30003',
           'node2': 'node36_30003',

           'port': '9080',

           'n1_color': 'green',
           'n2_color': 'green',

           'endpoint1':'172.16.10.1',
           'endpoint2':'172.16.10.12',

            'ep1_color': 'green',
            'ep2_color': 'red',

           'cluster_ip': '10.254.10.18:9080 [Failed]'
           }
    """

    dot_file_name = "dot/" + svc_name + ".dot"
    png_file_name = "png/" + svc_name + ".png"

    open(dot_file_name, "w").write(tpt.substitute(kv))

    cmd = 'dot -Tpng %s -o %s'%(dot_file_name, png_file_name)
    os.system(cmd)


def gen_svc_png(svc_name, node_list, ep_list, node_port, cluster_ip, port):
    template_header = '''
    digraph G {
        rankdir=LR
        compound=true
        style=filled

    '''
    template_ender = '}\n'


    subgrah_header = '''
        subgraph cluster0 {
                node [style=filled]
                style=solid
    '''

    subgrah_body = ''
    subgrah_ender = '}\n'


    for node in node_list:
        node_name = node + '_' + str(node_port)
        subgrah_body += '"%s"\n'%(node_name)

    cluster_ip_port = cluster_ip + ":" + str(port)
    cluster_ip_staus = " [OK]" if check_server(cluster_ip, port) else " [FAILED]"

    subgrah_body += 'label = "%s"\n'%(cluster_ip_port + " " + cluster_ip_staus)
    subgrah = subgrah_header + subgrah_body + subgrah_ender

    relation = ''
    for node in node_list:
        node_name = node + '_' + str(node_port)
        if node_port > 0:
            relation += 'nginx -> "%s"[color=%s]\n'%(node_name, 'green' if check_server(node, node_port) else 'red')
        else:
            relation += 'nginx -> "%s"[color=blue]\n' % (node_name)

        for ep in ep_list:
            relation += '"%s" -> "%s"[color=%s]\n'%(node_name, ep, 'green' if check_server(ep, port) else 'red')

    template_body = subgrah + relation
    cnt = template_header + template_body + template_ender

    dot_file_name = "dot/" + svc_name + ".dot"
    png_file_name = "png/" + svc_name + ".png"

    open(dot_file_name, "w").write(cnt)

    cmd = 'dot -Tpng %s -o %s'%(dot_file_name, png_file_name)
    os.system(cmd)



def check_svc(svc_name, node1, node2, nodeport, cluster_ip, port, ep1, ep2):
    # print "Enter into check svc"

    cluster_ip_status = ""
    if nodeport > 0:
        cluster_ip_status = " [OK]" if check_server(cluster_ip, port) else " [FAILED]"

    port_str = str(port)
    nodeport_str = str(nodeport)

    kv = {
      'node1': node1 + "_" + nodeport_str,
      'node2': node2 + "_" + nodeport_str,

      'port': port_str,

      'n1_color': 'green' if check_server(node1, nodeport) else 'red',
      'n2_color': 'green' if check_server(node2, nodeport) else 'red',

      'endpoint1': ep1,
      'endpoint2': ep2,

      'ep1_color': 'green' if check_server(ep1, port) else 'red',
      'ep2_color': 'green' if check_server(ep1, port) else 'red',

      'cluster_ip': cluster_ip + ":" + port_str + " " + cluster_ip_status  #'10.254.10.18:9080 [Failed]'
      }

    # print "Ready to call gen_svc_png "
    gen_svc_png(svc_name, kv)


def get_pods(host, port, pod_url):
    pod_list = []
    httpClient = None
    try:
        httpClient = httplib.HTTPConnection('139.196.231.45', 8080, timeout=3)
        httpClient.request('GET', pod_url)

        response = httpClient.getresponse()
        pod_infos = response.read()
        if response.status == 200:
            all_obj = json.loads(pod_infos)
            items = all_obj["items"]

            for item in items:
                pod = {}
                pod["node_name"] = item["spec"]["nodeName"]
                pod["status"] = item["status"]["phase"]
                pod["pod_ip"] = item["status"]["podIP"]

                pod_list.append(pod)

            return pod_list

        else:
            print response.status
            print response.reason

    except Exception, e:
        print e
    finally:
        if httpClient:
            httpClient.close()

if __name__ == '__main__':

    svc_list = {
        'activitysvc': ['eengoo-node6','eengoo-node7'],
        'albumsvc':    ['eengoo-node2','eengoo-node3'],
        'appsearchsvc': ['eengoo-node8','eengoo-node9'],
        'audiosvc': 	['eengoo-node4','eengoo-node5'],
        'chatletshopsvc': ['eengoo-node5', 'eengoo-node6'],
        'codepushsvc': 	['eengoo-node6','eengoo-node7'],
        'commentsvc': 	['eengoo-node5', 'eengoo-node6'],
        'eventlogsvc': 	['eengoo-node8', 'eengoo-node9'],
        'friendsvc': 		['eengoo-node2', 'eengoo-node3'],
        'groupsvc': 		['eengoo-node4', 'eengoo-node5'],
        'im-chat-svc': 	['eengoo-node6', 'eengoo-node7'],
        'im-connector-svc': ['eengoo-node6', 'eengoo-node7'],
        'im-master-svc': 	['eengoo-node6', 'eengoo-node7'],
        'im-push-svc': 		['eengoo-node6', 'eengoo-node7'],
        'im-recommend-svc': ['eengoo-node2', 'eengoo-node3'],
        'imagesvc': 	['eengoo-node8', 'eengoo-node9'],
        'mapsvc': 		['eengoo-node2', 'eengoo-node3'],
        'marketingsvc': ['eengoo-node2', 'eengoo-node3'],
        'pagesvc': 		['eengoo-node4', 'eengoo-node5'],
        'passportsvc': 	['eengoo-node6', 'eengoo-node7'],

        'pomelohttpsvc': ['eengoo-node1', 'eengoo-node2'],

        'postsvc': 	['eengoo-node2', 'eengoo-node3'],
        'recommendsvc': ['eengoo-node2', 'eengoo-node3'],
        'reportsvc': 	['eengoo-node8', 'eengoo-node9'],

        'sensitivewordssvc': ['eengoo-node1', 'eengoo-node2'],
        'statisticssvc': 	['eengoo-node1', 'eengoo-node2'],

        'tagsvc': 		['eengoo-node4', 'eengoo-node5'],
        'usersvc': 		['eengoo-node6', 'eengoo-node7'],
        'video-svc': 	['eengoo-node8', 'eengoo-node9'],
        'weathersvc': ['eengoo-node2', 'eengoo-node3']
    }

    # base_url = "http://139.196.231.45:8080/api/v1/namespaces/default/services/"
    svc_base_url = "/api/v1/namespaces/default/services/"
    pod_base_url = "/api/v1/namespaces/default/pods"

    api_host = "139.196.231.45"
    api_port = 8080

    # clean old data
    os.system("rm -rf dot/*")
    os.system("rm -rf png/*")

    for svc in sorted(svc_list):
        time.sleep(1)
        httpClient = None
        try:
            svc_url = svc_base_url + svc
            print "Dealing " + svc + "...." + api_host + ":" + str(api_port)

            httpClient = httplib.HTTPConnection(api_host, api_port, timeout=3)
            httpClient.request('GET', svc_url)

            response = httpClient.getresponse()
            svc_info = response.read()
            if response.status == 200:
                svc_obj = json.loads(svc_info)
                spec_obj = svc_obj["spec"]
                port0 = spec_obj["ports"][0]

                label = spec_obj["selector"]["yinnut-svc"]

                port = port0["port"]

                node_port = "0"
                if spec_obj["type"] == "NodePort":
                    node_port = port0["nodePort"]

                pod_url = pod_base_url

                if label:
                    pod_url += "?labelSelector=yinnut-svc="
                    pod_url += label

                pod_list = get_pods(api_host,api_port, pod_url)

                pod_str = "["
                for pod in pod_list:
                    pod_str += pod["node_name"]
                    pod_str += ": " + pod["pod_ip"]
                    pod_str += ", "

                pod_str += "]"

                cluster_ip = spec_obj["clusterIP"]

                # gen_svc_png(svc_name, node_list, ep_list, node_port, cluster_ip, port)
                node_list = svc_list[svc]

                ep_list = []
                for pod in pod_list:
                    ep_list.append(pod["pod_ip"])

                gen_svc_png(svc, node_list, ep_list, int(node_port), cluster_ip, int(port))

            else:
                print response.status
                print response.reason

        except Exception, e:
            print e
        finally:
            if httpClient:
                httpClient.close()
