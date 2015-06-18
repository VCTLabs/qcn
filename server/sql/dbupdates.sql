alter table host add gpu_active_frac double not null;

alter table result 
            add peak_working_set_size double not null,
            add peak_swap_size double not null,
            add peak_disk_usage double not null;


alter table app
            add fraction_done_exact tinyint not null;

      alter table app_version
            add beta tinyint not null;

        create table credit_user (
            userid                  integer         not null,
            appid                   integer         not null,
            njobs                   integer         not null,
            total                   double          not null,
            expavg                  double          not null,
            expavg_time             double          not null,
            credit_type             integer         not null,
            primary key (userid, appid, credit_type)
            ) engine=InnoDB;


     create table credit_team (
            teamid                  integer         not null,
            appid                   integer         not null,
            njobs                   integer         not null,
            total                   double          not null,
            expavg                  double          not null,
            expavg_time             double          not null,
            credit_type             integer         not null,
            primary key (teamid, appid, credit_type)
            ) engine=InnoDB;


    alter table user_submit add primary key(user_id);
    alter table user_submit_app add primary key(user_id, app_id);


    alter table forum
        alter timestamp set default 0,
        alter threads set default 0,
        alter posts set default 0,
        alter rate_min_expavg_credit set default 0,
        alter rate_min_total_credit set default 0,
        alter post_min_interval set default 0,
        alter post_min_expavg_credit set default 0,
        alter post_min_total_credit set default 0,
        alter parent_type set default 0;

============ rollback ------------

use sensor;

alter table app
            drop column fraction_done_exact;

alter table app_version
            drop column beta;

alter table host drop column gpu_active_frac;

lock tables result write;

alter table result 
     drop column peak_working_set_size,
     drop column peak_swap_size,
     drop column peak_disk_usage;

unlock tables;

