--TEST--
Generators using symtables must not leak
--FILE--
<?php

function gen()
{
    $bar = ["some complex var"];
    ${"f" . "oo"} = "force symtable usage";
    var_dump($bar);
    yield;
}
function fn1759069408()
{
    gen()->valid();
}
fn1759069408();
--EXPECT--
array(1) {
  [0]=>
  string(16) "some complex var"
}
