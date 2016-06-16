--TEST--
ZE2 Initializing static properties to arrays
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class test
{
    public static $ar = array();
}
function fn78398451()
{
    var_dump(test::$ar);
    test::$ar[] = 1;
    var_dump(test::$ar);
    echo "Done\n";
}
fn78398451();
--EXPECTF--
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
Done
