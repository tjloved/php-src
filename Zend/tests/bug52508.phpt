--TEST--
Bug #52508 (newline problem with parse_ini_file+INI_SCANNER_RAW)
--FILE--
<?php

function fn1098588268()
{
    $file = dirname(__FILE__) . '/bug52508.ini';
    file_put_contents($file, "a = 1");
    $ini_array = parse_ini_file($file, true, INI_SCANNER_RAW);
    var_dump($ini_array);
    unlink($file);
}
fn1098588268();
--EXPECT--
array(1) {
  ["a"]=>
  string(1) "1"
}
