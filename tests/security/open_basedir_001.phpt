--TEST--
openbase_dir runtime tightning
--SKIPIF--
<?php
if(PHP_OS_FAMILY === "Windows") {
	die('skip.. only for unix');
}
if (!is_dir("/usr/local/bin")) {
	die('skip.. no /usr/local/bin on this machine');
}
--INI--
open_basedir=/usr/local
--FILE--
<?php

function fn150427376()
{
    var_dump(ini_set("open_basedir", "/usr/local/bin"));
    var_dump(ini_get("open_basedir"));
    var_dump(ini_set("open_basedir", "/usr"));
    var_dump(ini_get("open_basedir"));
}
fn150427376();
--EXPECT--
string(10) "/usr/local"
string(14) "/usr/local/bin"
bool(false)
string(14) "/usr/local/bin"

