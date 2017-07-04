--TEST--
Testing multiples 'default:' in switch
--FILE--
<?php

function fn1022780628()
{
    switch (1) {
        case 2:
            print 'foo';
            break;
        case 3:
            print 'bar';
            break;
        default:
            print 1;
            break;
        default:
            print 2;
            break;
        default:
            print 3;
            break;
    }
}
fn1022780628();
--EXPECTF--
Fatal error: Switch statements may only contain one default clause in %s on line %d
