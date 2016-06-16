--TEST--
It's possible to invoke a generator dynamically
--FILE--
<?php

function gen($foo, $bar)
{
    (yield $foo);
    (yield $bar);
}
function fn943669449()
{
    $gen = call_user_func('gen', 'bar', 'foo');
    foreach ($gen as $value) {
        var_dump($value);
    }
}
fn943669449();
--EXPECT--
string(3) "bar"
string(3) "foo"
