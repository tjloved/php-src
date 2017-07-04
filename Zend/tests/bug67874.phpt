--TEST--
Bug #67874 Crash in array_map()
--FILE--
<?php

function fn630043199()
{
    $a = array(1, 2, 3);
    $data = array($a);
    $data = array_map('current', $data);
    var_dump($data);
}
fn630043199();
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
