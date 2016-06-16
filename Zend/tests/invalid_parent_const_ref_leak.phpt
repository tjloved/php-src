--TEST--
Leak when using an invalid parent:: reference in a constant definition
--FILE--
<?php

class A
{
    const B = parent::C;
}
function fn235895931()
{
    try {
        A::B;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
fn235895931();
--EXPECT--
Cannot access parent:: when current class scope has no parent
