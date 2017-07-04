--TEST--
ZE2 Initializing static properties to arrays
--FILE--
<?php

class test
{
    public static $ar = array();
}
function fn603828824()
{
    var_dump(test::$ar);
    test::$ar[] = 1;
    var_dump(test::$ar);
    echo "Done\n";
}
fn603828824();
--EXPECTF--
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
Done
