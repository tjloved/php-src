--TEST--
Bug #70681: Segfault when binding $this of internal instance method to null
--FILE--
<?php

function fn1823111413()
{
    $c = (new ReflectionMethod('SplStack', 'count'))->getClosure(new SplStack());
    $c = $c->bindTo(null);
    $c = (new ReflectionFunction('strlen'))->getClosure();
    $c = $c->bindTo(null);
    var_dump($c("foo"));
}
fn1823111413();
--EXPECTF--
Warning: Cannot unbind $this of internal method in %s on line %d
int(3)
