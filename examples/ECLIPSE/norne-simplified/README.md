# Simplified Norne Model

This deck is based on the Norne reservoir model.

It has been shortened and simplified to make it more readable and usable
for unit tests.

All the `WCONHIST` keywords have been replaced with `WCONPROD`, and most
of the controls have been outright removed.

## Wells

The wells remaining in the model are

| Well name | Type     | Open date  | N. Conns. | N. Controls |
| --------- | -------- | ---------- | --------- | ----------- |
| C-4H      | Injector | 1997-11-14 |  7        | 3/1
| B-2H      | Producer | 1997-12-01 |  8        | 2/1
| D-2H      | Producer | 1997-12-17 |  9        | 2/2
| B-4H      | Producer | 1998-04-23 | 20        | 3/1
| D-4H      | Producer | 1998-06-14 | 26        | 4/2
| C-1H      | Injector | 1998-07-16 | 21        | 3/1
| E-3H      | Producer | 1998-09-18 | 21        | 4/2
| C-2H      | Injector | 1999-01-21 |  3        | 2/2
| B-1H      | Producer | 1999-04-01 | 14        | 3/1
| C-3H      | Injector | 1999-05-20 | 21        | 4/1
| F-1H      | Injector | 1999-05-20 | 17        | 2/0

The N. Controls field indicates N. total/N. non-zero.
The open date is the time of the first non-zero control.
