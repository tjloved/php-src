--TEST--
unset() CV 11 (unset() of copy destoies original value)
--FILE--
<?php

function fn2080656980()
{
    $x = array("default" => "ok");
    var_dump($x);
    $cf = $x;
    unset($cf['default']);
    var_dump($x);
    echo "ok\n";
}
fn2080656980();
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
