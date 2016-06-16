--TEST--
Array keys can be yielded from generators
--FILE--
<?php

function gen()
{
    (yield [] => 1);
}
function fn2131001124()
{
    $gen = gen();
    var_dump($gen->key());
    var_dump($gen->current());
}
fn2131001124();
--EXPECT--
array(0) {
}
int(1)
