#include <stdlib.h>
#include "schedule.h"

Schedule schedule = {.num_slots = 17, .slots = {
    {.time = "08:00-09:00", .start_hour = 8, .start_min = 0, .title = "Reg & Breakfast", .is_small = 1},
    {.time = "09:00-09:20", .start_hour = 9, .start_min = 0, .title = "Welcome", .is_small = 1 },
    {.time = "09:20-09:50", .start_hour = 9, .start_min = 20, .num_sessions = 1, .sessions = {
      { .start = "09:20", .title = "Crossing the Language Chasm", .presenter = "David Calavera", .room = 1}
    }},
    {.time = "09:50-10:20", .start_hour = 9, .start_min = 50, .title = "Morning Refreshment", .is_small = 1 },

    {.time = "10:20-11:20", .start_hour = 10, .start_min = 20, .num_sessions = 2, .sessions = {
        {.start = "10:20", .title = "Stupid Gopher Tricks", .presenter = "Andrew Gerrand", .room = 1},
        {.start = "10:20", .title = "Building APIs", .presenter = "Mat Ryer", .room = 2},
    }},

    {.time = "11:20-11:30", .start_hour = 11, .start_min = 20, .title = "Room change", .is_small = 1},

    {.time = "11:30-12:30", .start_hour = 11, .start_min = 30, .num_sessions = 2, .sessions = {
        {.start = "11:30", .title = "Complex Concurrency Patterns in Go", .presenter = "Evan Huus", .room = 1},
        {.start = "11:30", .title = "Buinding a Bank with Go", .presenter = "Matt Heath", .room = 2},
    }},

    {.time = "12:30-13:30", .start_hour = 12, .start_min = 30, .title = "Lunch", .is_small = 1 },

    {.time = "13:30-14:30", .start_hour = 13, .start_min = 30, .num_sessions = 2, .sessions = {
        {.start = "13:30", .title = "Code Analysis", .presenter = "Francesc Campoy", .room = 1},
        {.start = "13:30", .title = "CockroachDB: Make Data Easy", .presenter = "Ben Darnell", .room = 2},
    }},

    {.time = "14:30-14:40", .start_hour = 14, .start_min = 30, .title = "Room change", .is_small = 1 },

    {.time = "14:40-15:40", .start_hour = 14, .start_min = 40, .num_sessions = 2, .sessions = {
        {.start = "14:40", .title = "Go Kit: A Toolkit for Microservices", .presenter = "Peter Bourgon", .room = 1},
        {.start = "14:40", .title = "Understanding Memory Allocation", .presenter = "Dean Elbaz", .room = 2},
    }},

    {.time = "15:40-16:00", .start_hour = 15, .start_min = 40, .title = "Afternoon Refreshment", .is_small = 1 },

    {.time = "16:00-17:00", .start_hour = 16, .start_min = 0, .num_sessions = 2, .sessions = {
        {.start = "16:00", .title = "Dependency Mgmt Conundrum", .presenter = "William Kennedy", .room = 1},
        {.start = "16:00", .title = "Whispered Secrets", .presenter = "Eleanor McHugh", .room = 2},
    }},

    {.time = "17:00-17:10", .start_hour = 17, .start_min = 0, .title = "Room change", .is_small = 1 },

    {.time = "17:10-17:40", .start_hour = 17, .start_min = 10, .num_sessions = 1, .sessions = {
        {.start = "17:10", .title = "The Go Community", .presenter = "Damian Gryski", .room = 1 }
    }},

    {.time = "17:40-18:00", .start_hour = 17, .start_min = 40, .title = "Prize Raffle & Closing", .is_small = 1 },

    {.time = "18:00", .start_hour = 18, .start_min = 0, .title = "Post-Conference Social", .is_small = 1 }
}};

void init_schedule() {
    Session *prev = NULL;
    for(int ts_idx=0; ts_idx<schedule.num_slots; ts_idx++) {
        TimeSlot *ts = &schedule.slots[ts_idx];
        if (!ts->num_sessions) {
            ts->is_common = 1;
            continue;
        }
        for(int s_idx=0; s_idx<ts->num_sessions; s_idx++) {
            Session *s = &ts->sessions[s_idx];
            s->prev = prev;
            s->next = NULL;
            if (prev) {
                prev->next = s;
            }
            prev = s;
        }
    }
}
