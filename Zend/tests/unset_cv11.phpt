--TEST--
unset() CV 11 (unset() of copy destoies original value)
--FILE--
<?php

function fn638485471()
{
    $x = array("default" => "ok");
    var_dump($x);
    $cf = $x;
    unset($cf['default']);
    var_dump($x);
    echo "ok\n";
}
fn638485471();
--EXPECT--
array(1) {
  ["default"]=>
  string(2) "ok"
}
array(1) {
  ["default"]=>
  string(2) "ok"
}
ok
