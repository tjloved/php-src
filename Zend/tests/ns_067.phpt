--TEST--
067: Name ambiguity (import name & internal class name)
--FILE--
<?php

function fn1942523183()
{
    include __DIR__ . '/ns_022.inc';
    include __DIR__ . '/ns_027.inc';
    include __DIR__ . '/ns_067.inc';
}
fn1942523183();
--EXPECT--
Foo\Bar\Foo
