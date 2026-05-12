#include <userver/components/component_list.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/redis/component.hpp>

#include "storage/storage_component.hpp"

#include "handlers/auth_register.hpp"
#include "handlers/auth_login.hpp"
#include "handlers/get_user_by_login.hpp"
#include "handlers/search_users.hpp"
#include "handlers/create_exercise.hpp"
#include "handlers/get_exercises.hpp"
#include "handlers/create_workout.hpp"
#include "handlers/add_exercise_to_workout.hpp"
#include "handlers/get_workout_history.hpp"
#include "handlers/get_workout_stats.hpp"

int main(int argc, char* argv[]) {
    if (RdKafka::init() != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to initialize librdkafka: " << RdKafka::err2str(RdKafka::last_error()) << std::endl;
        return 1;
    }

    try {
        userver::components::ComponentList component_list;

        component_list.Append<userver::components::MinimalServerComponentList>();

        component_list.Append<userver::storages::mongo::Client>();
        component_list.Append<userver::storages::redis::Client>();

        component_list.Append<storage::StorageComponent>();

        component_list.Append<handlers::AuthRegister>();
        component_list.Append<handlers::AuthLogin>();
        component_list.Append<handlers::GetUserByLogin>();
        component_list.Append<handlers::SearchUsers>();
        component_list.Append<handlers::CreateExercise>();
        component_list.Append<handlers::GetExercises>();
        component_list.Append<handlers::CreateWorkout>();
        component_list.Append<handlers::AddExerciseToWorkout>();
        component_list.Append<handlers::GetWorkoutHistory>();
        component_list.Append<handlers::GetWorkoutStats>();

        return userver::utils::DaemonRun(argc, argv, component_list);

    } catch (const std::exception& ex) {
        std::cerr << "Critical error: " << ex.what() << std::endl;
    }
    
    RdKafka::wait_destroyed(5000);
    return 0;
}