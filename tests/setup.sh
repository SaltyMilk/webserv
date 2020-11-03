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
}

function create_sample_file {
  echo "Give a name to the sample config file"
  read -rp "file: " file

  echo "Enter ip, port and server name for the config file"
  read -rp "ip: " ip
  read -rp "port: " port
  read -rp "name: " name

  touch $file
  printf "server\n{\n" >> $file
  #general server settings
  printf '\thost_addr %s\n\tserver_name %s\n\tport %s\n' "$ip" "$name" "$port" >> $file
  printf "\tcgi on\n\tcgi_ext .bla\n\tcgi_path tests/cgi_test\n\n" >> $file

  #locations
  printf "\tlocation /\n\t{\n\t\tallow_method GET\n\t}\n\t" >> $file
  printf "\tlocation /put_test/*\n\t{\n\t\tallow_method GET PUT\n\t\tupload_root_dir uploads\n\t}\n" >> $file
  printf "\tlocation /post_body\n\t{\n\t\tallow_method PUT\n\t\tbody_limit 100\n\t}\n" >> $file
  printf "\tlocation /directory/\n\t{\n\t\tallow_method GET\n\t\troot_directory tests/YoupiBanane\n\t}\n}" >> $file
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