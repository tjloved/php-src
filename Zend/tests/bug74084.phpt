--TEST--
Bug #74084 (Out of bound read - zend_mm_alloc_small)
--INI--
error_reporting=0
--FILE--
<?php

function fn1198270132()
{
    ${$A} += ${$B}->a =& ${$C};
    unset(${$A});
    ${$A} -= ${$B}->a =& ${$C};
    unset(${$A});
    ${$A} *= ${$B}->a =& ${$C};
    unset(${$A});
    ${$A} /= ${$B}->a =& ${$C};
    unset(${$A});
    ${$A} **= ${$B}->a =& ${$C};
    var_dump(${$A});
}
fn1198270132();
--EXPECT--
int(1)
