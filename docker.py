#!/usr/bin/env python
import string
import os


tag = "v1.2.0"
org_base_url = "index.tenxcloud.com/google_containers"
new_base_url = "dockerimages.yinnut.com:15043"

images = ["kube-apiserver", "kube-controller-manager", "kube-scheduler"]

for image in images:
    image_org_url = '%s/%s:%s'%(org_base_url, image, tag)
    image_new_url = '%s/%s:%s'%(new_base_url, image, tag)

    cmd_pull = 'docke pull ' + image_org_url
    print(cmd_pull)
    #os.system(cmd_pull)


    cmd_tag = 'docker tag %s %s'%(image_org_url,image_new_url)
    print(cmd_tag)
    #os.system(cmd_tag)

    cmd_push = 'docker push ' + image_new_url
    print(cmd_push)
    #os.system(cmd_push)
