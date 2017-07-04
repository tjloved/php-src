--TEST--
Calling generator through magic __call()
--FILE--
<?php

class A
{
    public function __call($name, $args)
    {
        for ($i = 0; $i < 5; $i++) {
            (yield $i);
        }
    }
}
function fn1149859388()
{
    $a = new A();
    foreach ($a->gen() as $n) {
        var_dump($n);
    }
    $a->gen();
}
fn1149859388();
--EXPECT--
int(0)
int(1)
int(2)
int(3)
int(4)
