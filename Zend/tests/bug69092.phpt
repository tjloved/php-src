--TEST--
Bug #69092 (Declare Encoding Compile Check Wrong)
--SKIPIF--
<?php
if (!extension_loaded("mbstring")) {
  die("skip Requires mbstring extension");
}
?>
--INI--
zend.multibyte=On
--FILE--
<?php

function foo()
{
    declare (encoding="utf-8");
}
function fn1136721787()
{
    echo "Hi";
    echo "Bye";
}
fn1136721787();
--EXPECTF--
Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line %d