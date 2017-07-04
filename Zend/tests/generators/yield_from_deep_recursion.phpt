--TEST--
Deep recursion with yield from
--FILE--
<?php

function from($i)
{
    (yield $i);
}
function gen($i = 0)
{
    if ($i < 50000) {
        yield from gen(++$i);
    } else {
        (yield $i);
        yield from from(++$i);
    }
}
function fn1815134740()
{
    ini_set("memory_limit", "512M");
    foreach (gen() as $v) {
        var_dump($v);
    }
}
fn1815134740();
--EXPECT--
int(50000)
int(50001)
