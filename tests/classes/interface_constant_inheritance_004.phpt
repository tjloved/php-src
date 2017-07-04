--TEST--
Ensure a class may implement two interfaces which include the same constant (due to inheritance). 
--FILE--
<?php

interface IA
{
    const FOO = 10;
}
interface IB extends IA
{
}
class C implements IA, IB
{
}
function fn1515029612()
{
    echo "Done\n";
}
fn1515029612();
--EXPECTF--
Done
