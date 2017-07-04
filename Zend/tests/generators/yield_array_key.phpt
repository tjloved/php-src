--TEST--
Array keys can be yielded from generators
--FILE--
<?php

function gen()
{
    (yield [] => 1);
}
function fn143430467()
{
    $gen = gen();
    var_dump($gen->key());
    var_dump($gen->current());
}
fn143430467();
--EXPECT--
array(0) {
}
int(1)
