#!/bin/bash

function create_structure {
  echo "Creating directories needed for tester..."
  mkdir -pv YoupiBanane
  mkdir -pv YoupiBanane/{nop,Yeah}

  echo "Creating files needed for tester..."
  cd YoupiBanane
  echo "This file is the youpi.bla file" >> youpi.bla
  echo "This file is the youpi.bad_extension file" >> youpi.bad_extension

  cd nop
  echo "This file is the youpi.bad_extension file in the nop directory" >> youpi.bad_extension
  echo "This file is the other.pouic file in the nop directory" >> other.pouic

  cd ../Yeah
  echo "This file is the not_happy.bad_extension file in the Yeah directory" >> not_happy.bad_extension

  cd ..
  cd ..
}

function create_sample_file {
  echo "Give a name to the sample config file"
  read -rp "file: " file

  echo "Enter ip, port, server name and cgi_test path for the config file"
  read -rp "ip: " ip
  read -rp "port: " port
  read -rp "name: " name
  read -e -rp "cgi_tester path: " cgi

  touch $file
  printf "server\n{\n" >> $file
  #general server settings
  printf 'host_addr %s\nserver_name %s\nport %s\n' "$ip" "$name" "$port" >> $file
  printf "cgi on\ncgi_ext .bla\ncgi_path %s\n\n" "$cgi" >> $file

  #locations
  printf "location /\n{\n\tallow_method GET\n}\n" >> $file
  printf "location /put_test/*\n{\n\tallow_method GET PUT\n\tupload_root_dir uploads\n}\n" >> $file
  printf "location /post_body\n{\n\tallow_method PUT\n\tbody_limit 100\n}\n" >> $file
  printf "location /directory/\n{\n\tallow_method GET\n\troot_directory tests/YoupiBanane\n}\n}" >> $file

  echo "File $file has successfully been created..."
}

echo "Do you want to install the directory structure ?"
select yn in "Yes" "No"; do
  case $yn in
      Yes ) create_structure; break;;
      No ) break;;
  esac
done

echo "Do you want to generate a sample config for the tester ?"
select yn in "Yes" "No"; do
  case $yn in
      Yes ) create_sample_file; exit;;
      No ) exit;;
  esac
done