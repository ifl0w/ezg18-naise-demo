pipeline {
	agent any

	stages {

        stage('Building Docker Image') {
            when {
                branch 'compile-docker-image'
            }
            steps {
                script {
                    def compilerImage = docker.build("iflow/naise-engine-cross-compile", ".")
                    compilerImage.push()
                }

            }
            post {
                success {
                    sh 'docker system prune -a -f'
                }
            }
        }

		stage('Build') {
			steps {
                sh './build-scripts/build_release.sh'
            }
		}

		stage('Master Release') {
            when {
                anyOf {
                    branch 'master';
                    branch 'develop';
                }
            }

            steps {
                sh './build-scripts/build_release.sh'
            }

			post {
                always {
                        archiveArtifacts 'release-win-x64/**/*'
                        archiveArtifacts 'release-linux-x64/**/*'
                }
            }
		}

	}

}